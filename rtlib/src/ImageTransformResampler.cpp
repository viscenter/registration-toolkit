#include "rt/ImageTransformResampler.hpp"

#include <itkNearestNeighborInterpolateImageFunction.h>

using namespace rt;

using ColorInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image8UC3, double>;

static constexpr uint8_t EMPTY_PIXEL = 0;

Image8UC3::Pointer rt::ImageTransformResampler(
    const Image8UC3::Pointer fixed,
    const Image8UC3::Pointer moving,
    ResampleFilter::TransformType* transform)
{
    auto interpolator = ColorInterpolator::New();
    auto resample = ResampleFilter::New();
    resample->SetInput(moving);
    resample->SetTransform(transform);
    resample->SetInterpolator(interpolator);
    resample->SetSize(fixed->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(fixed->GetOrigin());
    resample->SetOutputSpacing(fixed->GetSpacing());
    resample->SetOutputDirection(fixed->GetDirection());
    resample->SetDefaultPixelValue(EMPTY_PIXEL);
    resample->Update();

    return resample->GetOutput();
}