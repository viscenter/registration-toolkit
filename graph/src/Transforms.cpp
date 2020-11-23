#include "rt/graph/Transforms.hpp"

#include "rt/ImageTransformResampler.hpp"
#include "rt/util/ImageConversion.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

rtg::CompositeTransformNode::CompositeTransformNode()
{
    registerInputPort("first", first);
    registerInputPort("second", second);
    registerOutputPort("result", result);

    compute = [=]() {
        auto tfm = CompositeTransform::New();
        if (first_) {
            tfm->AddTransform(first_);
        }
        if (second_) {
            tfm->AddTransform(second_);
        }
        tfm->FlattenTransformQueue();
        result_ = tfm;
    };
}

smgl::Metadata rtg::CompositeTransformNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    if (useCache) {
        WriteTransform(cacheDir / "composite.tfm", result_);
        m["transform"] = "composite.tfm";
    }
    return m;
}

void rtg::CompositeTransformNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        result_ = ReadTransform(cacheDir / file);
    }
}

rtg::WriteTransformNode::WriteTransformNode()
{
    registerInputPort("path", path);
    registerInputPort("transform", transform);
    compute = [this]() {
        std::cout << "Writing transformation to file..." << std::endl;
        WriteTransform(path_, tfm_);
    };
}

smgl::Metadata rtg::WriteTransformNode::serialize_(bool, const fs::path&)
{
    return {{"path", path_.string()}};
}

void rtg::WriteTransformNode::deserialize_(
    const smgl::Metadata& meta, const fs::path&)
{
    path_ = meta["path"].get<std::string>();
}

rtg::TransformLandmarksNode::TransformLandmarksNode()
{
    registerInputPort("transform", transform);
    registerInputPort("landmarksIn", landmarksIn);
    registerOutputPort("landmarksOut", landmarksOut);

    compute = [this]() {
        ldmOut_.clear();
        auto i = tfm_->GetInverseTransform();
        for (const auto& p : ldmIn_) {
            ldmOut_.emplace_back(i->TransformPoint(p));
        }
    };
}

smgl::Metadata rtg::TransformLandmarksNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    // TODO: Implement
    return smgl::Metadata::object();
}

void rtg::TransformLandmarksNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    // TODO:: Implement
}

rtg::ImageResampleNode::ImageResampleNode()
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("transform", transform);
    registerOutputPort("resampledImage", resampledImage);

    compute = [=]() {
        // TODO: Don't do anything if the transform is identity
        cv::Mat tmp;
        auto cns = moving_.channels();
        if (forceAlpha_ and (cns == 1 or cns == 3)) {
            tmp = ColorConvertImage(moving_, cns + 1);
        } else {
            tmp = moving_;
        }
        std::cout << "Resampling image..." << std::endl;
        resampled_ = ImageTransformResampler(moving_, fixed_.size(), tfm_);
    };
}

smgl::Metadata rt::graph::ImageResampleNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    // TODO: Implement
    return smgl::Metadata::object();
}

void rt::graph::ImageResampleNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    // TODO: Implement
}
