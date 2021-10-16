#include "rt/graph/Transforms.hpp"

#include "rt/ImageTransformResampler.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/io/LandmarkIO.hpp"
#include "rt/io/UVMapIO.hpp"
#include "rt/util/ImageConversion.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

rtg::CompositeTransformNode::CompositeTransformNode() : Node{true}
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
    if (useCache and result_) {
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

rtg::TransformLandmarksNode::TransformLandmarksNode() : Node{true}
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
    smgl::Metadata m;
    if (useCache) {
        WriteLandmarkContainer(cacheDir / "transformed.lc", ldmOut_);
        m["landmarks"] = "transformed.lc";
    }
    return m;
}

void rtg::TransformLandmarksNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("landmarks")) {
        auto file = meta["landmarks"].get<std::string>();
        ldmOut_ = ReadLandmarkContainer(cacheDir / file);
    }
}

rtg::TransformUVMapNode::TransformUVMapNode() : Node{true}
{
    registerInputPort("transform", transform);
    registerInputPort("uvMapIn", uvMapIn);
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);

    registerOutputPort("uvMapOut", uvMapOut);

    compute = [this]() {
        std::cout << "Transform UV map..." << std::endl;
        uvOut_ = UVMap();
        uvOut_.ratio(fixed_.cols, fixed_.rows);
        uvOut_.setOrigin(uvIn_.origin());

        cv::Vec2d fixedSize{fixed_.cols - 1, fixed_.rows - 1};
        cv::Vec2d movingSize{moving_.cols - 1, moving_.rows - 1};
        for (const auto& [key, face] : uvIn_.faces_as_map()) {
            bool valid{true};
            UVMap::Face f;
            int fIdx{0};
            for (const auto& uv : uvIn_.getFaceUVs(key)) {
                // Transform the UV point
                auto in = uv.mul(fixedSize);
                auto out = tfm_->TransformPoint(in.val);
                cv::Vec2d newUV{out[0] / movingSize[0], out[1] / movingSize[1]};

                // Out-of-bounds UV
                if (newUV[0] < 0.0 or newUV[0] > 1.0 or newUV[1] < 0 or
                    newUV[1] > 1) {
                    valid = false;
                    break;
                }

                auto uvIdx = uvOut_.addUV(newUV);
                f[fIdx++] = uvIdx;
            }

            // Only add if we have a valid face
            if (valid) {
                uvOut_.addFace(key, f);
            }
        }
    };
}

smgl::Metadata rtg::TransformUVMapNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    if (useCache) {
        WriteUVMap(cacheDir / "transformed.uvm", uvOut_);
        m["uvMap"] = "transformed.uvm";
    }
    return m;
}

void rtg::TransformUVMapNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("uvMap")) {
        auto file = meta["uvMap"].get<std::string>();
        uvOut_ = ReadUVMap(cacheDir / file);
    }
}

rtg::ImageResampleNode::ImageResampleNode() : Node{true}
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("transform", transform);
    registerInputPort("forceAlpha", forceAlpha);
    registerOutputPort("resampledImage", resampledImage);

    compute = [=]() {
        cv::Mat tmp;
        auto cns = moving_.channels();
        if (forceAlpha_ and (cns == 1 or cns == 3)) {
            tmp = ColorConvertImage(moving_, cns + 1);
        } else {
            tmp = moving_;
        }
        std::cout << "Resampling image..." << std::endl;
        resampled_ = ImageTransformResampler(tmp, fixed_.size(), tfm_);
    };
}

smgl::Metadata rt::graph::ImageResampleNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    if (useCache and not resampled_.empty()) {
        WriteImage(cacheDir / "resampled.tif", resampled_);
        m["image"] = "resampled.tif";
    }
    return m;
}

void rt::graph::ImageResampleNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    if (meta.contains("image")) {
        auto file = meta["image"].get<std::string>();
        resampled_ = ReadImage(cacheDir / file);
    }
}
