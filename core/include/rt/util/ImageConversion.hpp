#pragma once

/** @file */

#include <opencv2/core.hpp>

namespace rt
{
/** @brief Convert image to specified depth using max scaling */
cv::Mat QuantizeImage(const cv::Mat& m, int depth = CV_16U);

/** @brief Convert image to specified number of channels */
cv::Mat ColorConvertImage(const cv::Mat& m, int channels = 1);

}  // namespace rt
