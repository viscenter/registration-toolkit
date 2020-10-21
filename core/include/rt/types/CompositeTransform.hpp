#pragma once

#include <boost/filesystem.hpp>

#include <itkCompositeTransform.h>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

using CompositeTransform = itk::CompositeTransform<double, 2>;
using CommonTransform = CompositeTransform::TransformType;

void WriteTransform(
    const boost::filesystem::path& path,
    const CommonTransform::Pointer& transform);
CompositeTransform::Pointer ReadTransform(const boost::filesystem::path& path);

namespace graph
{

class CompositeTransformNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    CompositeTransformNode();
    smgl::InputPort<CommonTransform::Pointer> lhs{&first_};
    smgl::InputPort<CommonTransform::Pointer> rhs{&second_};
    smgl::OutputPort<CommonTransform::Pointer> result{&combined_};

private:
    CommonTransform::Pointer first_;
    CommonTransform::Pointer second_;
    CommonTransform::Pointer combined_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class TransformLandmarksNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    TransformLandmarksNode();

    smgl::InputPort<CommonTransform::Pointer> transform{&tfm_};
    smgl::InputPort<LandmarkContainer> landmarksIn{&ldmIn_};
    smgl::OutputPort<LandmarkContainer> landmarksOut{&ldmOut_};

private:
    CommonTransform::Pointer tfm_;
    LandmarkContainer ldmIn_;
    LandmarkContainer ldmOut_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class WriteTransformNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    WriteTransformNode();

    smgl::InputPort<Path> path{&path_};
    smgl::InputPort<CommonTransform::Pointer> transform{&tfm_};

private:
    Path path_;
    CommonTransform::Pointer tfm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

}  // namespace graph
}  // namespace rt
