#pragma once

/** @file */

#include <smgl/Metadata.hpp>

#include "graph/DeformableRegistration.hpp"
#include "graph/ImageIO.hpp"
#include "graph/LandmarkIO.hpp"
#include "graph/LandmarkRegistration.hpp"
#include "graph/MeshIO.hpp"
#include "graph/MeshOps.hpp"
#include "graph/Transforms.hpp"

namespace rt::graph
{
/** @brief Register all rt nodes with the graph system */
auto RegisterNodes() -> bool;

/** @brief Get the standard registration-toolkit project metadata */
auto ProjectMetadata() -> smgl::Metadata;
}  // namespace rt::graph
