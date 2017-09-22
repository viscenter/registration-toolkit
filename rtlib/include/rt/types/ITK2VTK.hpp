#pragma once

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "rt/types/ITKMesh.hpp"

namespace rt
{
/**
 * @class ITK2VTK
 * @author Seth Parker
 * @date 8/3/15
 *
 * @brief Convert from an ITKMesh to VTK PolyData.
 *
 * Copy vertices, vertex normals, and faces (cells) from input to output.
 *
 * @see  examples/src/ITK2VTKExample.cpp
 *       meshing/test/ITK2VTKTest.cpp
 *
 * @ingroup types
 */
class ITK2VTK
{
public:
    ITK2VTK(ITKMesh::Pointer input, vtkSmartPointer<vtkPolyData> output);
};

/**
 * @class VTK2ITK
 * @author Seth Parker
 * @date 8/3/15
 *
 * @brief Convert from a VTK PolyData to an ITKMesh.
 *
 * Copy vertices, vertex normals, and faces (cells) from input to output.
 *
 * @see  examples/src/ITK2VTKExample.cpp
 *       meshing/test/ITK2VTKTest.cpp
 *
 * @ingroup types
 */
class VTK2ITK
{
public:
    VTK2ITK(vtkSmartPointer<vtkPolyData> input, ITKMesh::Pointer output);
};
}
