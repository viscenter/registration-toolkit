#include "rt/graph/DeformableRegistration.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

using Meta = smgl::Metadata;

rtg::DeformableRegistrationNode::DeformableRegistrationNode()
    : Node{true}
    , fixedImage{&reg_, &DeformableRegistration::setFixedImage}
    , movingImage{&reg_, &DeformableRegistration::setMovingImage}
    , meshFillSize{&reg_, &DeformableRegistration::setMeshFillSize}
    , gradientTolerance{&reg_, &DeformableRegistration::setGradientMagnitudeTolerance}
    , iterations{&iters_}
    , reportMetrics{&reg_, &DeformableRegistration::setReportMetrics}
    , transform{&tfm_}
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("iterations", iterations);
    registerInputPort("meshFillSize", meshFillSize);
    registerInputPort("gradientTolerance", gradientTolerance);
    registerInputPort("reportMetrics", reportMetrics);
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
    m["gradientTolerance"] = reg_.getGradientMagnitudeTolerance();
    m["reportMetrics"] = reg_.getReportMetrics();
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
    reg_.setGradientMagnitudeTolerance(meta["gradientTolerance"].get<double>());
    reg_.setReportMetrics(meta["reportMetrics"].get<bool>());
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        tfm_ = ReadTransform(cacheDir / file);
    }
}
