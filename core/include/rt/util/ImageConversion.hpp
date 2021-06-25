#pragma once

/** @file */

#include <opencv2/core.hpp>

namespace rt
{
/** @brief Convert image to specified depth using max scaling */
auto QuantizeImage(const cv::Mat& m, int depth = CV_16U) -> cv::Mat;

/** @brief Convert image to specified number of channels */
auto ColorConvertImage(const cv::Mat& m, int channels = 1) -> cv::Mat;

}  // namespace rt
