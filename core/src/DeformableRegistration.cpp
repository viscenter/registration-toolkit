#include "rt/DeformableRegistration.hpp"

#include <itkCommand.h>
#include <itkImageRegistrationMethod.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkRegularStepGradientDescentOptimizer.h>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;

using GrayInterpolator =
    itk::LinearInterpolateImageFunction<Image8UC1, double>;
using Metric =
    itk::MattesMutualInformationImageToImageMetric<Image8UC1, Image8UC1>;
using Optimizer = itk::RegularStepGradientDescentOptimizer;
using Registration = itk::ImageRegistrationMethod<Image8UC1, Image8UC1>;
using BSplineParameters = DeformableRegistration::Transform::ParametersType;

static constexpr double DEFAULT_MAX_STEP_FACTOR = 1.0 / 500.0;
static constexpr double DEFAULT_MIN_STEP_FACTOR = 1.0 / 500000.0;

/* The metric requires two parameters to be selected: the number
of bins used to compute the entropy and the number of spatial samples
used to compute the density estimates. In typical application, 50
histogram bins are sufficient and the metric is relatively insensitive
to changes in the number of bins. The number of spatial samples
to be used depends on the content of the image. If the images are
smooth and do not contain much detail, then using approximately
1 percent of the pixels will do. On the other hand, if the images
are detailed, it may be necessary to use a much higher proportion,
such as 20 percent. */
static constexpr size_t DEFAULT_HISTOGRAM_BINS = 50;
static constexpr double DEFAULT_SAMPLE_FACTOR = 1.0 / 80.0;

using Transform = DeformableRegistration::Transform;

class ReportMetricCallback : public itk::Command
{
protected:
    ReportMetricCallback() = default;

public:
    using Pointer = itk::SmartPointer<ReportMetricCallback>;
    using Optimizer = itk::RegularStepGradientDescentOptimizer;

    static auto New() -> Pointer
    {
        Pointer smartPtr = ::itk::ObjectFactory<ReportMetricCallback>::Create();
        if (smartPtr == nullptr) {
            smartPtr = new ReportMetricCallback;
        }
        smartPtr->UnRegister();
        return smartPtr;
    }

    void Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        Execute(reinterpret_cast<const itk::Object*>(caller), event);
    }

    void Execute(
        const itk::Object* object, const itk::EventObject& event) override
    {
        const auto* optimizer = dynamic_cast<const Optimizer*>(object);
        if (not itk::IterationEvent().CheckEvent(&event)) {
            return;
        }
        std::cout << optimizer->GetValue() << std::endl;
    }
};

void DeformableRegistration::setFixedImage(const cv::Mat& i)
{
    fixedImage_ = i;
}

void DeformableRegistration::setMovingImage(const cv::Mat& i)
{
    movingImage_ = i;
}

void DeformableRegistration::setNumberOfIterations(size_t i)
{
    iterations_ = i;
}

auto DeformableRegistration::getTransform() -> Transform::Pointer
{
    return output_;
}

void DeformableRegistration::setMeshFillSize(uint32_t i) { meshFillSize_ = i; }

auto DeformableRegistration::getMeshFillSize() const -> uint32_t
{
    return meshFillSize_;
}

void DeformableRegistration::setGradientMagnitudeTolerance(double i)
{
    gradientMagnitudeTolerance_ = i;
}

auto DeformableRegistration::getGradientMagnitudeTolerance() const -> double
{
    return gradientMagnitudeTolerance_;
}

void DeformableRegistration::setOutputMetric(bool i) { outputMetric_ = i; }

auto DeformableRegistration::getOutputMetric() const -> bool
{
    return outputMetric_;
}

auto DeformableRegistration::compute()
    -> DeformableRegistration::Transform::Pointer
{
    ///// Create grayscale images /////
    auto fixed8u = QuantizeImage(fixedImage_, CV_8U);
    auto fixed = CVMatToITKImage<Image8UC1>(fixed8u);
    auto moving8u = QuantizeImage(movingImage_, CV_8U);
    auto moving = CVMatToITKImage<Image8UC1>(moving8u);

    ///// Setup the BSpline Transform /////
    output_ = Transform::New();
    Transform::PhysicalDimensionsType fixedPhysicalDims;
    Transform::MeshSizeType meshSize;
    Transform::OriginType fixedOrigin;

    for (auto i = 0; i < 2; i++) {
        fixedOrigin[i] = fixed->GetOrigin()[i];
        fixedPhysicalDims[i] =
            fixed->GetSpacing()[i] *
            static_cast<double>(
                fixed->GetLargestPossibleRegion().GetSize()[i] - 1);
    }
    meshSize.Fill(meshFillSize_);

    output_->SetTransformDomainOrigin(fixedOrigin);
    output_->SetTransformDomainPhysicalDimensions(fixedPhysicalDims);
    output_->SetTransformDomainMeshSize(meshSize);
    output_->SetTransformDomainDirection(fixed->GetDirection());

    const auto numParams = output_->GetNumberOfParameters();
    BSplineParameters parameters(numParams);
    parameters.Fill(0.0);
    output_->SetParameters(parameters);

    ///// Setup Registration and Metrics /////
    auto metric = Metric::New();
    auto optimizer = Optimizer::New();
    auto registration = Registration::New();
    auto grayInterpolator = GrayInterpolator::New();
    if (outputMetric_) {
        optimizer->AddObserver(
            itk::IterationEvent(), ReportMetricCallback::New());
    }

    registration->SetFixedImage(fixed);
    registration->SetMovingImage(moving);
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInterpolator(grayInterpolator);
    registration->SetTransform(output_);
    registration->SetInitialTransformParameters(output_->GetParameters());

    auto fixedRegion = fixed->GetBufferedRegion();
    registration->SetFixedImageRegion(fixedRegion);

    metric->SetNumberOfHistogramBins(DEFAULT_HISTOGRAM_BINS);
    auto numSamples = static_cast<size_t>(
        fixedRegion.GetNumberOfPixels() * DEFAULT_SAMPLE_FACTOR);
    metric->SetNumberOfSpatialSamples(numSamples);

    ///// Setup Optimizer /////
    auto regionWidth =
        static_cast<double>(fixed->GetLargestPossibleRegion().GetSize()[0]);
    auto maxStepLength = regionWidth * DEFAULT_MAX_STEP_FACTOR;
    auto minStepLength = regionWidth * DEFAULT_MIN_STEP_FACTOR;

    optimizer->MinimizeOn();
    optimizer->SetMaximumStepLength(maxStepLength);
    optimizer->SetMinimumStepLength(minStepLength);
    optimizer->SetRelaxationFactor(relaxationFactor_);
    optimizer->SetNumberOfIterations(iterations_);
    optimizer->SetGradientMagnitudeTolerance(gradientMagnitudeTolerance_);

    ///// Run Registration /////
    registration->Update();

    // Report final values as requested
    if (outputMetric_) {
        std::cout << "Stop Condition: ";
        std::cout << optimizer->GetStopConditionDescription() << "\n";
        std::cout << "Final Metric Value:" << optimizer->GetValue() << "\n";
    }

    output_->SetParameters(registration->GetLastTransformParameters());
    return output_;
}