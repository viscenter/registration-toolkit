#pragma once

#include <itkResampleImageFilter.h>

#include "rt/ImageTypes.hpp"

namespace rt
{

using ResampleFilter = itk::ResampleImageFilter<Image8UC3, Image8UC3, double>;

Image8UC3::Pointer ImageTransformResampler(
    const Image8UC3::Pointer fixed,
    const Image8UC3::Pointer moving,
    ResampleFilter::TransformType* transform);
}