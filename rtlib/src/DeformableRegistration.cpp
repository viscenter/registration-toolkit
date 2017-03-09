#include "rt/DeformableRegistration.hpp"

#include <itkImageRegistrationMethod.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkResampleImageFilter.h>

using namespace rt;

using GrayInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image8UC1, double>;
using GrayscaleFilter = itk::RGBToLuminanceImageFilter<Image8UC3, Image8UC1>;
using ResampleFilter = itk::ResampleImageFilter<Image8UC3, Image8UC3, double>;
using ColorInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image8UC3, double>;
using Metric =
    itk::MattesMutualInformationImageToImageMetric<Image8UC1, Image8UC1>;
using Optimizer = itk::RegularStepGradientDescentOptimizer;
using Registration = itk::ImageRegistrationMethod<Image8UC1, Image8UC1>;
using BSplineParameters = DeformableRegistration::Transform::ParametersType;

static constexpr double DEFAULT_MAX_STEP_FACTOR = 1.0 / 500.0;
static constexpr double DEFAULT_MIN_STEP_FACTOR = 1.0 / 500000.0;
static constexpr unsigned DEFAULT_MESH_FILL_SIZE = 12;
static constexpr uint8_t EMPTY_PIXEL = 0;

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

DeformableRegistration::Transform::Pointer DeformableRegistration::compute()
{
    ///// Create grayscale images /////
    GrayscaleFilter::Pointer fixedFilter = GrayscaleFilter::New();
    fixedFilter->SetInput(fixedImage_);

    GrayscaleFilter::Pointer movingFilter = GrayscaleFilter::New();
    movingFilter->SetInput(movingImage_);

    ///// Setup the BSpline Transform /////
    output_ = Transform::New();
    Transform::PhysicalDimensionsType FixedPhysicalDims;
    Transform::MeshSizeType MeshSize;
    Transform::OriginType FixedOrigin;

    for (auto i = 0; i < 2; i++) {
        FixedOrigin[i] = fixedImage_->GetOrigin()[i];
        FixedPhysicalDims[i] =
            fixedImage_->GetSpacing()[i] *
            (fixedImage_->GetLargestPossibleRegion().GetSize()[i] - 1);
    }
    MeshSize.Fill(DEFAULT_MESH_FILL_SIZE);

    output_->SetTransformDomainOrigin(FixedOrigin);
    output_->SetTransformDomainPhysicalDimensions(FixedPhysicalDims);
    output_->SetTransformDomainMeshSize(MeshSize);
    output_->SetTransformDomainDirection(fixedImage_->GetDirection());

    const auto numParams = output_->GetNumberOfParameters();
    BSplineParameters parameters(numParams);
    parameters.Fill(0.0);
    output_->SetParameters(parameters);

    ///// Setup Registration and Metrics/////
    Metric::Pointer metric = Metric::New();
    Optimizer::Pointer optimizer = Optimizer::New();
    Registration::Pointer registration = Registration::New();
    GrayInterpolator::Pointer grayInterpolator = GrayInterpolator::New();

    registration->SetFixedImage(fixedFilter->GetOutput());
    registration->SetMovingImage(movingFilter->GetOutput());
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInterpolator(grayInterpolator);
    registration->SetTransform(output_);
    registration->SetInitialTransformParameters(output_->GetParameters());

    Image8UC3::RegionType fixedRegion = fixedImage_->GetBufferedRegion();
    registration->SetFixedImageRegion(fixedRegion);

    metric->SetNumberOfHistogramBins(DEFAULT_HISTOGRAM_BINS);
    auto numSamples = static_cast<size_t>(
        fixedRegion.GetNumberOfPixels() * DEFAULT_SAMPLE_FACTOR);
    metric->SetNumberOfSpatialSamples(numSamples);

    ///// Setup Optimizer /////
    auto regionWidth = fixedImage_->GetLargestPossibleRegion().GetSize()[0];
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

Image8UC3::Pointer DeformableRegistration::getTransformedImage()
{
    auto interpolator = ColorInterpolator::New();
    auto resample = ResampleFilter::New();
    resample->SetInput(movingImage_);
    resample->SetTransform(output_);
    resample->SetInterpolator(interpolator);
    resample->SetSize(fixedImage_->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(fixedImage_->GetOrigin());
    resample->SetOutputSpacing(fixedImage_->GetSpacing());
    resample->SetOutputDirection(fixedImage_->GetDirection());
    resample->SetDefaultPixelValue(EMPTY_PIXEL);
    resample->Update();

    return resample->GetOutput();
}