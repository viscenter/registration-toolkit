#include "rt/graph/LandmarkRegistration.hpp"

#include "rt/io/LandmarkIO.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

rtg::LandmarkDetectorNode::LandmarkDetectorNode() : Node{true}
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerOutputPort("fixedLandmarks", fixedLandmarks);
    registerOutputPort("movingLandmarks", movingLandmarks);
    compute = [this]() {
        std::cout << "Detecting landmarks..." << std::endl;
        detector_.setFixedImage(fixedImg_);
        detector_.setMovingImage(movingImg_);
        detector_.compute();
        fixedLdm_ = detector_.getFixedLandmarks();
        movingLdm_ = detector_.getMovingLandmarks();
    };
}

smgl::Metadata rtg::LandmarkDetectorNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m{{"matchRatio", detector_.matchRatio()}};
    if (useCache) {
        LandmarkWriter writer;
        writer.setPath(cacheDir / "landmarks.ldm");
        writer.setFixedLandmarks(fixedLdm_);
        writer.setMovingLandmarks(movingLdm_);
        writer.write();
        m["landmarks"] = "landmarks.ldm";
    }

    return m;
}

void rtg::LandmarkDetectorNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    detector_.setMatchRatio(meta["matchRatio"].get<float>());
    if (meta.contains("landmarks")) {
        auto file = meta["landmarks"].get<std::string>();
        LandmarkReader reader;
        reader.setLandmarksPath(cacheDir / file);
        reader.read();
        fixedLdm_ = reader.getFixedLandmarks();
        movingLdm_ = reader.getMovingLandmarks();
    }
}

rtg::AffineLandmarkRegistrationNode::AffineLandmarkRegistrationNode()
    : Node{true}
{
    registerInputPort("fixedLandmarks", fixedLandmarks);
    registerInputPort("movingLandmarks", movingLandmarks);
    registerInputPort("outputMetric", outputMetric);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running affine registration..." << std::endl;
        reg_.setFixedLandmarks(fixed_);
        reg_.setMovingLandmarks(moving_);
        tfm_ = reg_.compute();
    };
}

smgl::Metadata rtg::AffineLandmarkRegistrationNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    if (useCache and tfm_) {
        WriteTransform(cacheDir / "affine.tfm", tfm_);
        m["transform"] = "affine.tfm";
    }

    return m;
}

void rtg::AffineLandmarkRegistrationNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        tfm_ = ReadTransform(cacheDir / file);
    }
}

rtg::BSplineLandmarkWarpingNode::BSplineLandmarkWarpingNode() : Node{true}
{
    registerInputPort("fixedLandmarks", fixedLandmarks);
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingLandmarks", movingLandmarks);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running B-spline landmark registration..." << std::endl;
        reg_.setFixedLandmarks(fixed_);
        reg_.setFixedImage(fixedImg_);
        reg_.setMovingLandmarks(moving_);
        tfm_ = reg_.compute();
    };
}

smgl::Metadata rtg::BSplineLandmarkWarpingNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    if (useCache and tfm_) {
        WriteTransform(cacheDir / "bspline.tfm", tfm_);
        m["transform"] = "bspline.tfm";
    }

    return m;
}

void rtg::BSplineLandmarkWarpingNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        tfm_ = ReadTransform(cacheDir / file);
    }
}