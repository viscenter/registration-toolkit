#pragma once

#include <boost/filesystem.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/io/LandmarkReader.hpp"
#include "rt/io/LandmarkWriter.hpp"

namespace rt
{
namespace graph
{

class LandmarkReaderNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    LandmarkReaderNode();
    smgl::InputPort<Path> path{&path_};
    smgl::OutputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::OutputPort<LandmarkContainer> movingLandmarks{&moving_};

private:
    LandmarkReader reader_;
    Path path_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

class LandmarkWriterNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    LandmarkWriterNode();
    smgl::InputPort<Path> path{&path_};
    smgl::InputPort<LandmarkContainer> fixed{&fixed_};
    smgl::InputPort<LandmarkContainer> moving{&moving_};

private:
    LandmarkWriter writer_;
    Path path_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

}  // namespace graph
}  // namespace rt