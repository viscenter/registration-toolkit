#include "rt/graph/DeformableRegistration.hpp"

namespace rtg = rt::graph;

rtg::DeformableRegistrationNode::DeformableRegistrationNode()
{
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingImage", movingImage);
    registerInputPort("iterations", iterations);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running deformable registration..." << std::endl;
        tfm_ = reg_.compute();
    };
}