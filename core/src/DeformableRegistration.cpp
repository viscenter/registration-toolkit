#include "rt/DeformableRegistration.hpp"

#include <itkImageMaskSpatialObject.h>
#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizer.h>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;

using GrayInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image8UC1, double>;
using Metric =
    itk::MattesMutualInformationImageToImageMetric<Image8UC1, Image8UC1>;
using Optimizer = itk::RegularStepGradientDescentOptimizer;
using Registration = itk::ImageRegistrationMethod<Image8UC1, Image8UC1>;
using BSplineParameters = DeformableRegistration::Transform::ParametersType;
using Mask = itk::ImageMaskSpatialObject<2>;

static constexpr double DEFAULT_MAX_STEP_FACTOR = 1.0 / 500.0;
static constexpr double DEFAULT_MIN_STEP_FACTOR = 1.0 / 500000.0;
static constexpr unsigned DEFAULT_MESH_FILL_SIZE = 12;

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

using Transform = DeformableRegistration::Transform;

void DeformableRegistration::setFixedImage(const cv::Mat& i)
{
    fixedImage_ = i;
}

void DeformableRegistration::setMovingImage(const cv::Mat& i)
{
    movingImage_ = i;
}

void DeformableRegistration::setFixedMask(const cv::Mat& i) { fixedMask_ = i; }

void DeformableRegistration::setMovingMask(const cv::Mat& i)
{
    movingMask_ = i;
}

void DeformableRegistration::setNumberOfIterations(size_t i)
{
    iterations_ = i;
}

void DeformableRegistration::setSampleFactor(double f)
{
    // Bound to [0.1, 1.0]%
    sampleFactor_ = std::min(std::max(f, 0.01), 1.0);
}

Transform::Pointer DeformableRegistration::getTransform() { return output_; }

DeformableRegistration::Transform::Pointer DeformableRegistration::compute()
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
    meshSize.Fill(DEFAULT_MESH_FILL_SIZE);

    output_->SetTransformDomainOrigin(fixedOrigin);
    output_->SetTransformDomainPhysicalDimensions(fixedPhysicalDims);
    output_->SetTransformDomainMeshSize(meshSize);
    output_->SetTransformDomainDirection(fixed->GetDirection());

    BSplineParameters parameters(output_->GetNumberOfParameters());
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

    auto region = fixed->GetBufferedRegion();
    registration->SetFixedImageRegion(region);

    metric->SetNumberOfHistogramBins(DEFAULT_HISTOGRAM_BINS);
    auto numSamples =
        static_cast<size_t>(region.GetNumberOfPixels() * sampleFactor_);
    metric->SetNumberOfSpatialSamples(numSamples);

    if (not fixedMask_.empty()) {
        auto mask = Mask::New();
        mask->SetImage(rt::CVMatToITKImage<Image8UC1>(fixedMask_));
        metric->SetFixedImageMask(mask);
    }

    if (not movingMask_.empty()) {
        auto mask = Mask::New();
        mask->SetImage(rt::CVMatToITKImage<Image8UC1>(movingMask_));
        metric->SetMovingImageMask(mask);
    }

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

    output_->SetParameters(registration->GetLastTransformParameters());
    return output_;
}