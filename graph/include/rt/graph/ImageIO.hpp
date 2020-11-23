#pragma once

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/filesystem.hpp"

namespace rt
{
namespace graph
{

class ImageReadNode : public smgl::Node
{
    using Path = filesystem::path;
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
    using Path = filesystem::path;
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