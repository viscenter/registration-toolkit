#pragma once

/** @file */

#include <opencv2/core.hpp>

#include "rt/types/Transforms.hpp"

namespace rt
{

/**
 * @brief Resample a moving image using a pre-generated transform. Output image
 * is of size s.
 *
 */
cv::Mat ImageTransformResampler(
    const cv::Mat& m, const cv::Size& s, const Transform::Pointer& transform);
}  // namespace rt