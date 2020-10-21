#include "rt/graph/Transforms.hpp"

#include "rt/ImageTransformResampler.hpp"
#include "rt/util/ImageConversion.hpp"

namespace rtg = rt::graph;

rtg::CompositeTransformNode::CompositeTransformNode()
{
    registerInputPort("lhs", lhs);
    registerInputPort("rhs", rhs);
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
        combined_ = tfm;
    };
}

smgl::Metadata rtg::CompositeTransformNode::serialize_(
    bool useCache, const Path& cacheDir)
{
    Metadata m;
    if (useCache) {
        WriteTransform(cacheDir / "composite.tfm", combined_);
        m["transform"] = "composite.tfm";
    }
    return m;
}

void rtg::CompositeTransformNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        combined_ = ReadTransform(cacheDir / file);
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

smgl::Metadata rtg::WriteTransformNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::WriteTransformNode::deserialize_(const Metadata& meta, const Path&)
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
    bool useCache, const Path& cacheDir)
{
    // TODO: Implement
    return Metadata::object();
}

void rtg::TransformLandmarksNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
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