#pragma once

/** @file */

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "rt/types/ITKMesh.hpp"

namespace rt
{
/**
 * @brief Convert from an ITKMesh to VTK PolyData.
 *
 * Copy vertices, vertex normals, and faces (cells) from input to output.
 *
 */
void ITK2VTK(ITKMesh::Pointer input, vtkSmartPointer<vtkPolyData> output);

/**
 * @brief Convert from a VTK PolyData to an ITKMesh.
 *
 * Copy vertices, vertex normals, and faces (cells) from input to output.
 *
 */
void VTK2ITK(vtkSmartPointer<vtkPolyData> input, ITKMesh::Pointer output);
}  // namespace rt
