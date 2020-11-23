#pragma once

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/LandmarkRegistrationBase.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt
{
namespace graph
{

class CompositeTransformNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

public:
    CompositeTransformNode();
    smgl::InputPort<Transform::Pointer> lhs{&first_};
    smgl::InputPort<Transform::Pointer> rhs{&second_};
    smgl::OutputPort<Transform::Pointer> result{&combined_};

private:
    Transform::Pointer first_;
    Transform::Pointer second_;
    Transform::Pointer combined_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class WriteTransformNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

public:
    WriteTransformNode();

    smgl::InputPort<Path> path{&path_};
    smgl::InputPort<Transform::Pointer> transform{&tfm_};

private:
    Path path_;
    Transform::Pointer tfm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class TransformLandmarksNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

public:
    TransformLandmarksNode();

    smgl::InputPort<Transform::Pointer> transform{&tfm_};
    smgl::InputPort<LandmarkContainer> landmarksIn{&ldmIn_};
    smgl::OutputPort<LandmarkContainer> landmarksOut{&ldmOut_};

private:
    Transform::Pointer tfm_;
    LandmarkContainer ldmIn_;
    LandmarkContainer ldmOut_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class ImageResampleNode : public smgl::Node
{
public:
    ImageResampleNode();

    smgl::InputPort<cv::Mat> fixedImage{&fixed_};
    smgl::InputPort<cv::Mat> movingImage{&moving_};
    smgl::InputPort<Transform::Pointer> transform{&tfm_};
    smgl::InputPort<bool> forceAlpha{&forceAlpha_};
    smgl::OutputPort<cv::Mat> resampledImage{&resampled_};

private:
    bool forceAlpha_{false};
    cv::Mat fixed_;
    cv::Mat moving_;
    Transform::Pointer tfm_;
    cv::Mat resampled_;

    // TODO: Serialize
};

}  // namespace graph
}  // namespace rt