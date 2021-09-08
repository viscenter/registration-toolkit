#include "rt/graph.hpp"

#include <smgl/Node.hpp>

using namespace rt;
using namespace rt::graph;

static auto RegisterNodesImpl() -> bool
{
    bool registered{true};

    // clang-format off
    // ImageIO
    registered &= smgl::RegisterNode<ImageReadNode, ImageWriteNode>();

    // Landmark Registration
    registered &= smgl::RegisterNode<
        CompositeTransformNode,
        LandmarkReaderNode,
        LandmarkDetectorNode,
        LandmarkWriterNode,
        AffineLandmarkRegistrationNode,
        BSplineLandmarkWarpingNode>();

    // Transforms
    registered &= smgl::RegisterNode<
        ImageResampleNode,
        TransformLandmarksNode,
        WriteTransformNode,
        TransformUVMapNode>();

    // Deformable Registration
    registered &= smgl::RegisterNode<DeformableRegistrationNode>();

    // MeshIO
    registered &= smgl::RegisterNode<MeshReadNode, MeshWriteNode>();

    // MeshOps
    registered &= smgl::RegisterNode<ReorderTextureNode>();
    // clang-format on

    return registered;
}

void rt::graph::RegisterNodes()
{
    static auto registered = RegisterNodesImpl();
}
