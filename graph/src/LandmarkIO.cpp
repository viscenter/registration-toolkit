#include "rt/graph/LandmarkIO.hpp"

namespace rtg = rt::graph;

rtg::LandmarkReaderNode::LandmarkReaderNode()
{
    registerInputPort("path", path);
    registerOutputPort("fixedLandmarks", fixedLandmarks);
    registerOutputPort("movingLandmarks", movingLandmarks);
    compute = [this]() {
        std::cout << "Loading landmarks from file..." << std::endl;
        reader_.setLandmarksPath(path_);
        reader_.read();
        fixed_ = reader_.getFixedLandmarks();
        moving_ = reader_.getMovingLandmarks();
    };
}

smgl::Metadata rtg::LandmarkReaderNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::LandmarkReaderNode::deserialize_(const Metadata& meta, const Path&)
{
    path_ = meta["path"].get<std::string>();
    compute();
}

rtg::LandmarkWriterNode::LandmarkWriterNode()
{
    registerInputPort("path", path);
    registerInputPort("fixed", fixed);
    registerInputPort("moving", moving);
    compute = [this]() {
        std::cout << "Writing landmarks to file..." << std::endl;
        writer_.setPath(path_);
        writer_.setFixedLandmarks(fixed_);
        writer_.setMovingLandmarks(moving_);
        writer_.write();
    };
}

smgl::Metadata rtg::LandmarkWriterNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::LandmarkWriterNode::deserialize_(const Metadata& meta, const Path&)
{
    path_ = meta["path"].get<std::string>();
}
