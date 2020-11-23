#pragma once

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/AffineLandmarkRegistration.hpp"
#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/LandmarkDetector.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt
{
namespace graph
{

class LandmarkDetectorNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

public:
    LandmarkDetectorNode();
    smgl::InputPort<cv::Mat> fixedImage{&fixedImg_};
    smgl::InputPort<cv::Mat> movingImage{&movingImg_};
    smgl::OutputPort<LandmarkContainer> fixedLandmarks{&fixedLdm_};
    smgl::OutputPort<LandmarkContainer> movingLandmarks{&movingLdm_};

private:
    LandmarkDetector detector_;
    cv::Mat fixedImg_;
    cv::Mat movingImg_;
    LandmarkContainer fixedLdm_;
    LandmarkContainer movingLdm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class AffineLandmarkRegistrationNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

public:
    AffineLandmarkRegistrationNode();

    smgl::InputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::InputPort<LandmarkContainer> movingLandmarks{&moving_};
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};

private:
    AffineLandmarkRegistration reg_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;
    Transform::Pointer tfm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

class BSplineLandmarkWarpingNode : public smgl::Node
{
    using Path = filesystem::path;
    using Metadata = smgl::Metadata;

    LandmarkContainer fixed_;
    cv::Mat fixedImg_;
    LandmarkContainer moving_;
    Transform::Pointer tfm_;

public:
    BSplineLandmarkWarpingNode();

    smgl::InputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::InputPort<cv::Mat> fixedImage{&fixedImg_};
    smgl::InputPort<LandmarkContainer> movingLandmarks{&moving_};
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};

private:
    BSplineLandmarkWarping reg_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

}  // namespace graph
}  // namespace rt