#pragma once

#include <itkCompositeTransform.h>
#include <opencv2/core.hpp>

namespace rt
{

/**
 * @brief Resample a moving image using a pre-generated transform. Output image
 * is of size s.
 *
 * @ingroup rtlib
 */
cv::Mat ImageTransformResampler(
    const cv::Mat& m,
    const cv::Size& s,
    itk::CompositeTransform<double, 2>::Pointer transform);
}