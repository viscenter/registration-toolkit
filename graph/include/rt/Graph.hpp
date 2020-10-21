#pragma once

#include "rt/graph/DeformableRegistration.hpp"
#include "rt/graph/ImageIO.hpp"
#include "rt/graph/LandmarkIO.hpp"
#include "rt/graph/LandmarkRegistration.hpp"
#include "rt/graph/Transforms.hpp"

namespace rt
{
namespace graph
{
inline void RegisterAllNodeTypes()
{
    smgl::RegisterNode<ImageReadNode>();
    smgl::RegisterNode<ImageWriteNode>();
    smgl::RegisterNode<CompositeTransformNode>();
    smgl::RegisterNode<LandmarkReaderNode>();
    smgl::RegisterNode<LandmarkDetectorNode>();
    smgl::RegisterNode<LandmarkWriterNode>();
    smgl::RegisterNode<AffineLandmarkRegistrationNode>();
    smgl::RegisterNode<TransformLandmarksNode>();
    smgl::RegisterNode<BSplineLandmarkWarpingNode>();
    smgl::RegisterNode<ImageResampleNode>();
    smgl::RegisterNode<DeformableRegistrationNode>();
    smgl::RegisterNode<WriteTransformNode>();
}
}  // namespace graph
}  // namespace rt
