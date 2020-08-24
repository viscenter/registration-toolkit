#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace rt
{

/** @brief Write image to the specified path */
void WriteImage(const boost::filesystem::path& path, const cv::Mat& img);

}  // namespace rt