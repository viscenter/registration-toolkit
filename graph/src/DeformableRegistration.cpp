#include "rt/graph/DeformableRegistration.hpp"

namespace rtg = rt::graph;
namespace fs = rt::filesystem;

using Meta = smgl::Metadata;

rtg::DeformableRegistrationNode::DeformableRegistrationNode()
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("iterations", iterations);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running deformable registration..." << std::endl;
        reg_.setNumberOfIterations(iters_);
        tfm_ = reg_.compute();
    };
}

smgl::Metadata rtg::DeformableRegistrationNode::serialize_(
    bool useCache, const fs::path& cacheDir)
{
    Meta m;
    m["iterations"] = iters_;
    if (useCache) {
        // TODO: Serialize transform
    }
    return m;
}

void rtg::DeformableRegistrationNode::deserialize_(
    const Meta& meta, const fs::path& cacheDir)
{
    iters_ = meta["iterations"].get<int>();
}
