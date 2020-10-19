#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

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

namespace graph
{

class ImageReadNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    ImageReadNode();
    smgl::InputPort<Path> path{&path_};
    smgl::OutputPort<cv::Mat> image{&img_};

private:
    Path path_;
    cv::Mat img_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

class ImageWriteNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    ImageWriteNode();
    smgl::InputPort<Path> path{&path_};
    smgl::InputPort<cv::Mat> image{&img_};

private:
    Path path_;
    cv::Mat img_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

}  // namespace graph
}  // namespace rt