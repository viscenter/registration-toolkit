#include "rt/graph/MeshOps.hpp"

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/io/UVMapIO.hpp"

using namespace rt;

namespace fs = rt::filesystem;
namespace rtg = rt::graph;

rtg::ReorderTextureNode::ReorderTextureNode() : Node{true}
{
    registerInputPort("mesh", meshIn);
    registerInputPort("imageIn", imageIn);
    registerInputPort("uvMapIn", uvMapIn);
    registerInputPort("sampleRate", sampleRate);
    registerOutputPort("imageOut", imageOut);
    registerOutputPort("uvMapOut", uvMapOut);

    compute = [this]() {
        std::cout << "Reordering texture image...\n";
        rt::ReorderUnorganizedTexture op;
        op.setMesh(mesh_);
        op.setUVMap(inUV_);
        op.setTextureMat(inImg_);
        op.setSampleRate(sample_rate_);
        outImg_ = op.compute();
        outUV_ = op.getUVMap();
    };
}

smgl::Metadata rtg::ReorderTextureNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    smgl::Metadata m;
    m["sampleRate"] = sample_rate_;
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
    sample_rate_ = meta["sampleRate"].get<double>();
    if (meta.contains("uvMap")) {
        auto file = meta["uvMap"].get<std::string>();
        outUV_ = ReadUVMap(cacheDir / file);
    }
    if (meta.contains("image")) {
        auto file = meta["image"].get<std::string>();
        outImg_ = ReadImage(cacheDir / file);
    }
}
