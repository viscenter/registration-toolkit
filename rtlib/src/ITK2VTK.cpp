//
// Created by Seth Parker on 8/3/15.
//
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include <array>

#include "rt/types/ITK2VTK.hpp"

namespace rt
{
///// ITK Mesh -> VTK Polydata /////
ITK2VTK::ITK2VTK(ITKMesh::Pointer input, vtkSmartPointer<vtkPolyData> output)
{

    // points + normals
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto pointNormals = vtkSmartPointer<vtkDoubleArray>::New();
    pointNormals->SetNumberOfComponents(3);  // 3d normals (ie x,y,z)

    for (auto point = input->GetPoints()->Begin();
         point != input->GetPoints()->End(); ++point) {
        // assign the point
        points->InsertPoint(
            point->Index(), point->Value()[0], point->Value()[1],
            point->Value()[2]);

        // assign the normal
        ITKPixel normal;
        if (input->GetPointData(point.Index(), &normal)) {
            std::array<double, 3> ptNorm = {normal[0], normal[1], normal[2]};
            pointNormals->InsertTuple(point->Index(), ptNorm.data());
        }
    }

    // cells
    auto polys = vtkSmartPointer<vtkCellArray>::New();
    for (auto cell = input->GetCells()->Begin();
         cell != input->GetCells()->End(); ++cell) {

        auto poly = vtkSmartPointer<vtkIdList>::New();
        for (auto point = cell.Value()->PointIdsBegin();
             point != cell.Value()->PointIdsEnd(); ++point) {
            poly->InsertNextId(*point);
        }

        polys->InsertNextCell(poly);
    }

    // assign to the mesh
    output->SetPoints(points);
    output->SetPolys(polys);
    if (pointNormals->GetNumberOfTuples() > 0) {
        output->GetPointData()->SetNormals(pointNormals);
    }
}

///// VTK Polydata -> ITK Mesh /////
VTK2ITK::VTK2ITK(vtkSmartPointer<vtkPolyData> input, ITKMesh::Pointer output)
{

    // points + normals
    auto pointNormals = input->GetPointData()->GetNormals();
    for (vtkIdType pointId = 0; pointId < input->GetNumberOfPoints();
         ++pointId) {
        auto point = input->GetPoint(pointId);
        output->SetPoint(pointId, point);
        if (pointNormals != nullptr) {
            auto normal = pointNormals->GetTuple(pointId);
            output->SetPointData(pointId, normal);
        }
    }

    // cells
    ITKCell::CellAutoPointer cell;
    for (vtkIdType cellId = 0; cellId < input->GetNumberOfCells(); ++cellId) {
        auto inputCell = input->GetCell(cellId);  // input cell
        cell.TakeOwnership(new ITKTriangle);      // output cell

        for (vtkIdType pointId = 0; pointId < inputCell->GetNumberOfPoints();
             ++pointId) {
            cell->SetPointId(
                pointId,
                inputCell->GetPointId(pointId));  // assign the point id's
        }

        output->SetCell(cellId, cell);
    }
};
}
