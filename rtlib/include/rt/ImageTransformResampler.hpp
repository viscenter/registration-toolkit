#pragma once

#include <itkResampleImageFilter.h>

#include "rt/ImageTypes.hpp"

namespace rt
{
/** Resample filter type */
using ResampleFilter = itk::ResampleImageFilter<Image8UC3, Image8UC3, double>;

/** @brief Resample a moving image to match a moving image using a transform
 * generated by a registration algorithm
 */
Image8UC3::Pointer ImageTransformResampler(
    const Image8UC3::Pointer fixed,
    const Image8UC3::Pointer moving,
    ResampleFilter::TransformType* transform);
}