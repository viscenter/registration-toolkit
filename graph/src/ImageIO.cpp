#include "rt/graph/ImageIO.hpp"

#include "rt/io/ImageIO.hpp"

namespace rtg = rt::graph;

rtg::ImageReadNode::ImageReadNode()
{
    registerInputPort("path", path);
    registerOutputPort("image", image);
    compute = [this]() { img_ = ReadImage(path_); };
}

smgl::Metadata rtg::ImageReadNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::ImageReadNode::deserialize_(const Metadata& meta, const Path&)
{
    path_ = meta["path"].get<std::string>();
    img_ = rt::ReadImage(path_);
}

rtg::ImageWriteNode::ImageWriteNode()
{
    registerInputPort("path", path);
    registerInputPort("image", image);
    compute = [this]() { WriteImage(path_, img_); };
}

smgl::Metadata rtg::ImageWriteNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::ImageWriteNode::deserialize_(const Metadata& meta, const Path&)
{
    path_ = meta["path"].get<std::string>();
}