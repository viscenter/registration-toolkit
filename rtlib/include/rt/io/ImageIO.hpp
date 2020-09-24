#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace rt
{

/**
 * @brief Write image to the specified path
 *
 * Use rt::WriteTIFF for all tiff images, which includes support for
 * transparency and floating-point images. Otherwise, uses cv::imwrite.
 */
void WriteImage(const boost::filesystem::path& path, const cv::Mat& img);

}  // namespace rt