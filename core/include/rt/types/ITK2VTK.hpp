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
void ITK2VTK(
    const ITKMesh::Pointer& input, vtkSmartPointer<vtkPolyData>& output);

/** @copydoc ITK2VTK */
vtkSmartPointer<vtkPolyData> ITK2VTK(const ITKMesh::Pointer& input);

/**
 * @brief Convert from a VTK PolyData to an ITKMesh.
 *
 * Copy vertices, vertex normals, and faces (cells) from input to output.
 *
 */
void VTK2ITK(
    const vtkSmartPointer<vtkPolyData>& input, ITKMesh::Pointer& output);

/** @copydoc VTK2ITK */
ITKMesh::Pointer VTK2ITK(const vtkSmartPointer<vtkPolyData>& input);
}  // namespace rt
