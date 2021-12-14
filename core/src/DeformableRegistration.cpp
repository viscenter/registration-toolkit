#include "rt/DeformableRegistration.hpp"

#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
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

void DeformableRegistration::setMeshFillSize(unsigned i) { meshFillSize_ = i; }

unsigned DeformableRegistration::getMeshFillSize() { return meshFillSize_; }

void DeformableRegistration::setGradientMagnitudeTolerance(double i)
{
    gradientMagnitudeTolerance_ = i;
}

double DeformableRegistration::getGradientMagnitudeTolerance()
{
    return gradientMagnitudeTolerance_;
}

void DeformableRegistration::setOutputMetric(bool i) { outputMetric_ = i; }

bool DeformableRegistration::getOutputMetric() { return outputMetric_; }

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

    ///// Setup Registration and Metrics/////
    auto metric = Metric::New();
    auto optimizer = Optimizer::New();
    auto registration = Registration::New();
    auto grayInterpolator = GrayInterpolator::New();

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
    auto regionWidth = fixed->GetLargestPossibleRegion().GetSize()[0];
    auto maxStepLength = regionWidth * DEFAULT_MAX_STEP_FACTOR;
    auto minStepLength = regionWidth * DEFAULT_MIN_STEP_FACTOR;

    optimizer->MinimizeOn();
    optimizer->SetMaximumStepLength(maxStepLength);
    optimizer->SetMinimumStepLength(minStepLength);
    optimizer->SetRelaxationFactor(relaxationFactor_);
    optimizer->SetNumberOfIterations(iterations_);
    optimizer->SetGradientMagnitudeTolerance(gradientMagnitudeTolerance_);

    // checking to see if they want the metric output
    if (outputMetric_) {
        CommandIterationUpdate::Pointer observer =
            CommandIterationUpdate::New();
        optimizer->AddObserver(itk::IterationEvent(), observer);

        ///// Run Registration /////
        registration->Update();

        std::cout << " Stop Condition: "
                  << optimizer->GetStopConditionDescription() << std::endl;
        std::cout << "Final Metric Value:" << optimizer->GetValue()
                  << std::endl;
    } else {
        ///// Run Registration /////
        registration->Update();
    }

    output_->SetParameters(registration->GetLastTransformParameters());
    return output_;
}