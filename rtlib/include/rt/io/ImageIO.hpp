#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace rt
{

/**
 * @brief Read an image from the specified path
 *
 * Currently just a wrapper around:
 *
 * @code
 * cv::imread(path.string(), cv::IMREAD_UNCHANGED);
 * @endcode
 */
cv::Mat ReadImage(const boost::filesystem::path& path);

/**
 * @brief Write image to the specified path
 *
 * Use rt::WriteTIFF for all tiff images, which includes support for
 * transparency and floating-point images. Otherwise, uses cv::imwrite.
 */
void WriteImage(const boost::filesystem::path& path, const cv::Mat& img);

}  // namespace rt