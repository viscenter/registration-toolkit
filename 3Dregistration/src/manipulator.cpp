#include "manipulator.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOBBTree.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLandmarkTransform.h>
#include <vtkPLYWriter.h>
#include <vtkMatrix4x4.h>
#include <math.h>
#include <opencv2/core.hpp>

Manipulator::Manipulator() 
{
    file_path = "";
}

Manipulator::Manipulator(std::string &file) 
{
    file_path = file;
}

bool Manipulator::ReadObj(vtkSmartPointer<vtkOBJPolyDataProcessor> reader)
{
    bool success = false;
    // Check to see if the Manipulator has been initialized with a file path
    if (file_path != "") {
        // Set the file path for the reader and get the mesh (obj)
        reader->SetFileName(file_path.c_str());
        reader->SetMTLfileName("/Users/ztan222/source/registration-toolkit/3Dregistration/build/PHerc118-Pezzo2-SharpMesh-Textured.mtl");
        reader->Update();
        success = true;
    }
    return success;
}

void Manipulator::WriteObj(vtkSmartPointer<vtkTransformPolyDataFilter> mesh)
{
    // vtkSmartPointer<vtkPolyDataMapper> mapper =
    // vtkSmartPointer<vtkPolyDataMapper>::New();
    // mapper->SetInputConnection(mesh->GetOutputPort());

    // vtkSmartPointer<vtkActor> actor =
    // vtkSmartPointer<vtkActor>::New();
    // actor->SetMapper(mapper);

    // vtkSmartPointer<vtkRenderer> renderer =
    // vtkSmartPointer<vtkRenderer>::New();
    // renderer->AddActor(actor);
    // renderer->SetBackground(.3, .3, .6); // Background color blue

    // vtkSmartPointer<vtkRenderWindow> renderWindow =
    // vtkSmartPointer<vtkRenderWindow>::New();
    // renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkPLYWriter> writer =
        vtkSmartPointer<vtkPLYWriter>::New();
    writer->SetInputData(mesh->GetOutput());
    writer->SetFileName("results.ply");
    writer->Write();
}

bool Manipulator::ManipulateObj()
{
    bool success = true;
    vtkSmartPointer<vtkOBJPolyDataProcessor> reader = vtkSmartPointer<vtkOBJPolyDataProcessor>::New();
    if (!ReadObj(reader)) {
        return false;
    }

    vtkSmartPointer<vtkPolyData> data = reader->GetOutput(0);

    std::cout << "Reader filename: " << reader->GetFileName() << std::endl;
    vtkSmartPointer<vtkOBBTree> obb_tree = vtkSmartPointer<vtkOBBTree>::New();
    double corner[3]; double max[3]; double mid[3]; double min[3]; double size[3];
    // Computes the OBB and returns the 3 axis relative to the box
    obb_tree->ComputeOBB(data, corner, max, mid, min, size);

    // Convert to points of space; We assume that the min array is the "z axis"
    cv::Vec3d c(corner);
    cv::Vec3d o_x(max), o_y(mid), o_z(min);

    o_x *= 1/cv::norm(o_x);
    o_y *= 1/cv::norm(o_y);
    o_z *= 1/cv::norm(o_z);

    o_x += c;
    o_y += c;
    o_z += c;

    double m_cc[3], m_i[3], m_j[3], m_k[3];
    m_cc[0] = c[0];
    m_cc[1] = c[1];
    m_cc[2] = c[2];

    m_i[0] = o_x[0];
    m_i[1] = o_x[1];
    m_i[2] = o_x[2];

    m_j[0] = o_y[0];
    m_j[1] = o_y[1];
    m_j[2] = o_y[2];

    m_k[0] = o_z[0];
    m_k[1] = o_z[1];
    m_k[2] = o_z[2];

    // std::vector<ITKPointType> moving_points;
    // moving_points.push_back(m_cc); 
    // moving_points.push_back(m_i);
    // moving_points.push_back(m_j);
    // moving_points.push_back(m_k);

    vtkSmartPointer<vtkPoints> moving_points = vtkSmartPointer<vtkPoints>::New();
    moving_points->InsertNextPoint(m_cc);
    moving_points->InsertNextPoint(m_i);
    moving_points->InsertNextPoint(m_j);
    moving_points->InsertNextPoint(m_k);

    double f_cc[3], f_i[3], f_j[3], f_k[3];
    f_cc[0] = 0.0;
    f_cc[1] = 0.0;
    f_cc[2] = 0.0;

    f_i[0] = 1.0;
    f_i[1] = 0.0;
    f_i[2] = 0.0;

    f_j[0] = 0.0;
    f_j[1] = 1.0;
    f_j[2] = 0.0;

    f_k[0] = 0.0;
    f_k[1] = 0.0;
    f_k[2] = 1.0;

    // std::vector<ITKPointType> fixed_points;
    // fixed_points.push_back(f_cc);
    // fixed_points.push_back(f_i);
    // fixed_points.push_back(f_j);
    // fixed_points.push_back(f_k);

    vtkSmartPointer<vtkPoints> fixed_points = vtkSmartPointer<vtkPoints>::New();
    fixed_points->InsertNextPoint(f_cc);
    fixed_points->InsertNextPoint(f_i);
    fixed_points->InsertNextPoint(f_j);
    fixed_points->InsertNextPoint(f_k);

    vtkSmartPointer<vtkTransformPolyDataFilter> aligned_mesh = AlignObj(data, moving_points, fixed_points);
    WriteObj(aligned_mesh);

    return success;
}

vtkSmartPointer<vtkTransformPolyDataFilter> Manipulator::AlignObj(vtkSmartPointer<vtkPolyData> reader, vtkSmartPointer<vtkPoints> moving_points, vtkSmartPointer<vtkPoints> fixed_points)
{
    vtkSmartPointer<vtkLandmarkTransform> landmark_transform = vtkSmartPointer<vtkLandmarkTransform>::New();
    landmark_transform->SetSourceLandmarks(moving_points);
    landmark_transform->SetTargetLandmarks(fixed_points);
    landmark_transform->Update();


    vtkSmartPointer<vtkTransformPolyDataFilter> transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform_filter->SetInputData(reader);
    transform_filter->SetTransform(landmark_transform);
    transform_filter->Update(); 

    vtkMatrix4x4* mat = landmark_transform->GetMatrix();
    std::cout << *mat << std::endl;
    return transform_filter;
}

void Manipulator::VisualizeResults(vtkSmartPointer<vtkOBJImporter> reader)
{
    std::cout << "Number of items" << reader->GetRenderer()->GetActors()->GetNumberOfItems();

    vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(reader->GetRenderer()->GetActors()->GetLastActor()->GetMapper());

     vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);
  renderer->SetBackground(.3, .6, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
 
  renderWindowInteractor->Start();
}

void Manipulator::SetObjFilePath(std::string &file)
{
    file_path = file;
}


std::string Manipulator::GetObjFilePath()
{
    return file_path;
}
