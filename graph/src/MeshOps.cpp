#include "rt/graph/MeshOps.hpp"

#include "rt/io/ImageIO.hpp"
#include "rt/io/UVMapIO.hpp"

using namespace rt;

namespace fs = rt::filesystem;
namespace rtg = rt::graph;

// Enum conversions
namespace rt::graph
{
// clang-format off
using SamplingOrigin = ReorderTextureNode::SamplingOrigin;
NLOHMANN_JSON_SERIALIZE_ENUM(SamplingOrigin, {
    {SamplingOrigin::TopLeft, "top-left"},
    {SamplingOrigin::TopRight, "top-right"},
    {SamplingOrigin::BottomLeft, "bottom-left"},
    {SamplingOrigin::BottomRight, "bottom-right"}
})

using SamplingMode = ReorderTextureNode::SamplingMode;
NLOHMANN_JSON_SERIALIZE_ENUM(SamplingMode, {
    {SamplingMode::Rate, "rate"},
    {SamplingMode::OutputWidth, "width"},
    {SamplingMode::OutputHeight, "height"},
    {SamplingMode::AutoUV, "auto"},
})
// clang-format on
}  // namespace rt::graph

rtg::ReorderTextureNode::ReorderTextureNode()
    : Node{true}
    , meshIn{&reorder_, &ReorderUnorganizedTexture::setMesh}
    , imageIn{&reorder_, &ReorderUnorganizedTexture::setTextureMat}
    , uvMapIn{&reorder_, &ReorderUnorganizedTexture::setUVMap}
    , samplingOrigin{&reorder_, &ReorderUnorganizedTexture::setSamplingOrigin}
    , samplingMode{&reorder_, &ReorderUnorganizedTexture::setSamplingMode}
    , sampleRate{&reorder_, &ReorderUnorganizedTexture::setSampleRate}
    , sampleDim{&reorder_, &ReorderUnorganizedTexture::setSampleDim}
    , useFirstIntersection{&reorder_, &ReorderUnorganizedTexture::setUseFirstIntersection}
    , imageOut{&outImg_}
    , uvMapOut{&outUV_}
{
    registerInputPort("mesh", meshIn);
    registerInputPort("imageIn", imageIn);
    registerInputPort("uvMapIn", uvMapIn);
    registerInputPort("samplingOrigin", samplingOrigin);
    registerInputPort("samplingMode", samplingMode);
    registerInputPort("sampleRate", sampleRate);
    registerInputPort("sampleDim", sampleDim);
    registerInputPort("useFirstIntersection", useFirstIntersection);
    registerOutputPort("imageOut", imageOut);
    registerOutputPort("uvMapOut", uvMapOut);

    compute = [this]() {
        std::cout << "Reordering texture image...\n";
        outImg_ = reorder_.compute();
        outUV_ = reorder_.getUVMap();
    };
}

auto rtg::ReorderTextureNode::serialize_(
    bool useCache, const fs::path& cacheDir) -> smgl::Metadata
{
    smgl::Metadata m{
        {"samplingOrigin", reorder_.samplingOrigin()},
        {"samplingMode", reorder_.samplingMode()},
        {"sampleRate", reorder_.sampleRate()},
        {"sampleDim", reorder_.sampleDim()},
        {"useFirstIntersection", reorder_.useFirstIntersection()},
    };
    if (useCache) {
        if (not outUV_.empty()) {
            WriteUVMap(cacheDir / "reordered_uv.uvm", outUV_);
            m["uvMap"] = "reordered_uv.uvm";
        }
        if (not outImg_.empty()) {
            WriteImage(cacheDir / "reordered_img.tif", outImg_);
            m["image"] = "reordered_img.tif";
        }
    }
    return m;
}

void rtg::ReorderTextureNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    reorder_.setSamplingOrigin(meta["samplingOrigin"].get<SamplingOrigin>());
    reorder_.setSamplingMode(meta["samplingMode"].get<SamplingMode>());
    reorder_.setSampleRate(meta["sampleRate"].get<double>());
    reorder_.setSampleDim(meta["sampleDim"].get<std::size_t>());
    reorder_.setUseFirstIntersection(meta["useFirstIntersection"].get<bool>());
    if (meta.contains("uvMap")) {
        auto file = meta["uvMap"].get<std::string>();
        outUV_ = ReadUVMap(cacheDir / file);
    }
    if (meta.contains("image")) {
        auto file = meta["image"].get<std::string>();
        outImg_ = ReadImage(cacheDir / file);
    }
}
