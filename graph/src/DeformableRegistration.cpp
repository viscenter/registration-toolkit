#include "rt/graph/DeformableRegistration.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

using Meta = smgl::Metadata;

rtg::DeformableRegistrationNode::DeformableRegistrationNode() : Node{true}
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("iterations", iterations);
    registerInputPort("meshFillSize", meshFillSize);
    registerInputPort("gradientMagnitudeTolerance", gradientMagnitudeTolerance);
    registerInputPort("outputMetric", outputMetric);
    registerOutputPort("transform", transform);
    

    compute = [=]() {
        std::cout << "Running deformable registration..." << std::endl;
        reg_.setNumberOfIterations(iters_);
        tfm_ = reg_.compute();
    };
}

auto rtg::DeformableRegistrationNode::serialize_(
    bool useCache, const fs::path& cacheDir) -> smgl::Metadata
{
    Meta m;
    m["iterations"] = iters_;
    m["meshFillSize"] = reg_.getMeshFillSize();
    m["gradientMagnitudeTolerance"] = reg_.getGradientMagnitudeTolerance();
    if (useCache and tfm_) {
        WriteTransform(cacheDir / "deformable.tfm", tfm_);
        m["transform"] = "deformable.tfm";
    }
    return m;
}

void rtg::DeformableRegistrationNode::deserialize_(
    const Meta& meta, const fs::path& cacheDir)
{
    iters_ = meta["iterations"].get<int>();
    reg_.setMeshFillSize(meta["meshFillSize"].get<unsigned>());
    reg_.setGradientMagnitudeTolerance(meta["gradientMagnitudeTolerance"].get<double>());
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        tfm_ = ReadTransform(cacheDir / file);
    }
}
