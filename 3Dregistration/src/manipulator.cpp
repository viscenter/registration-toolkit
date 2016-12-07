#include "manipulator.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOBBTree.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLandmarkTransform.h>
#include <vtkPLYWriter.h>
#include <vtkMatrix4x4.h>
#include <math.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>

Manipulator::Manipulator() 
{
    _file_path = "";
}

Manipulator::Manipulator(std::string &file) 
{
    _file_path = file;
    _uv_reader = OBJReader();
}

bool Manipulator::ReadObj(vtkSmartPointer<vtkOBJReader> reader)
{
    bool success = false;
    // Check to see if the Manipulator has been initialized with a file path
    if (_file_path != "") {
        // Set the file path for the reader and get the mesh (obj)
        std::cout << "Reading files..." << std::endl;
        _texture_img = cv::imread(_file_path + "-Textured_0.png", 1);
        if (!_texture_img.data) {
            std::cout << "Problem loading texture image data" << std::endl;
        }
        _uv_reader.Parse(_file_path + "-Textured.obj");
        reader->SetFileName((_file_path + ".obj").c_str());
        reader->Update();
        success = true;
    }
    return success;
}

void Manipulator::WriteObj(vtkSmartPointer<vtkPolyData> mesh)
{
    vtkSmartPointer<vtkPLYWriter> writer =
        vtkSmartPointer<vtkPLYWriter>::New();
    writer->SetInputData(mesh);
    writer->SetFileName("results.ply");
    writer->Write();
}

bool Manipulator::ManipulateObj()
{
    bool success = true;
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    if (!ReadObj(reader)) {
        return false;
    }
    vtkSmartPointer<vtkPolyData> aligned_data = AlignObj(reader->GetOutput());
    vtkSmartPointer<vtkPolyData> topmost_data = FindIntersections(aligned_data);
    WriteObj(topmost_data);
    return success;
}

vtkSmartPointer<vtkPolyData> Manipulator::FindIntersections(vtkSmartPointer<vtkPolyData> aligned_data)
{
    vtkSmartPointer<vtkOBBTree> obb_tree = vtkSmartPointer<vtkOBBTree>::New();
    double corner[3]; double max[3]; double mid[3]; double min[3]; double size[3];
    // Computes the OBB and returns the 3 axis relative to the box
    obb_tree->ComputeOBB(aligned_data, corner, max, mid, min, size);
    obb_tree->SetDataSet(aligned_data); 
    obb_tree->BuildLocator();

    cv::Vec3d c(corner);
    cv::Vec3d o_x(max), o_y(mid), o_z(min);

    std::cout << "Corner: " << c << std::endl;
    std::cout << "X " << o_x << std::endl;
    std::cout << "Y " << o_y << std::endl;
    std::cout << "Z " << o_z << std::endl;

 
    vtkSmartPointer<vtkPoints> intersection_points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> intersection_result = vtkSmartPointer<vtkPolyData>::New();
    double samplerate = 1.0;
    auto rows = std::ceil((o_y[1] + 1) / samplerate);
    auto cols = std::ceil((o_x[0] + 1) / samplerate);
    cv::Mat texture_img_result = cv::Mat::zeros(rows, cols, CV_8UC3);
    for (auto j = 0; j < rows; j += samplerate)
    {
        for (auto i = 0; i < cols; i += samplerate) 
        {
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
            double a0[3] = {(double)i, (double)j, o_z[2]};
            double a1[3] = {(double)i, (double)j, c[2]};
            int code = obb_tree->IntersectWithLine(a0, a1, points, cellIds);
            if (code != 0)
            {
                vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
                aligned_data->GetCellPoints(cellIds->GetId(cellIds->GetNumberOfIds() - 1), pointIds);

                assert(pointIds->GetNumberOfIds() == 3); 
                vtkIdType p_id0 = pointIds->GetId(0);
                vtkIdType p_id1 = pointIds->GetId(1);
                vtkIdType p_id2 = pointIds->GetId(2);

                double p0[3]; double p1[3]; double p2[3];
                aligned_data->GetPoint(p_id0, p0);
                aligned_data->GetPoint(p_id1, p1);
                aligned_data->GetPoint(p_id2, p2);

                double bounds[6];
                points->GetBounds(bounds);
                double intersection_point[3] = {(double)i, (double)j, bounds[5]};
                intersection_points->InsertNextPoint(intersection_point);

                // Convert each point to a Vec3d for simple conversions
                cv::Vec3d A(p0), B(p1), C(p2), alpha(intersection_point);
                cv::Vec3d bary_point = BarycentricCoord(alpha, A, B, C);

                cv::Vec2d a(_uv_reader.GetUV(p_id0)), b(_uv_reader.GetUV(p_id1)), c(_uv_reader.GetUV(p_id2));
                A[0] = a[0];
                A[1] = a[1];
                A[2] = 0.0;

                B[0] = b[0];
                B[1] = b[1];
                B[2] = 0.0;

                C[0] = c[0];
                C[1] = c[1];
                C[2] = 0.0;

                cv::Vec3d cart_point = CartesianCoord(bary_point, A, B, C);
                cart_point[0] *= _texture_img.size().width;
                cart_point[1] *= _texture_img.size().height;

                uint16_t x = std::round(cart_point[0]);
                uint16_t y = std::round(cart_point[1]);

                auto pixel_color = _texture_img.at<cv::Vec3b>(y, x);
                std::cout <<  "Pixel: " << i << "," << j << " | Cartesian Point: " << x << "," << y << " | " << pixel_color << "\r" << std::flush;
                texture_img_result.at<cv::Vec3b>(j/samplerate, i/samplerate) = pixel_color;
            }
        }
    }
    std::cout << std::endl;    
    cv::imwrite(_file_path + "-New-Textured.png", texture_img_result);
    intersection_result->SetPoints(intersection_points);
    return intersection_result;

}

vtkSmartPointer<vtkPolyData> Manipulator::AlignObj(vtkSmartPointer<vtkPolyData> data)
{ 

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

    vtkSmartPointer<vtkPoints> fixed_points = vtkSmartPointer<vtkPoints>::New();
    fixed_points->InsertNextPoint(f_cc);
    fixed_points->InsertNextPoint(f_i);
    fixed_points->InsertNextPoint(f_j);
    fixed_points->InsertNextPoint(f_k);
    vtkSmartPointer<vtkLandmarkTransform> landmark_transform = vtkSmartPointer<vtkLandmarkTransform>::New();
    landmark_transform->SetSourceLandmarks(moving_points);
    landmark_transform->SetTargetLandmarks(fixed_points);
    landmark_transform->Update();


    vtkSmartPointer<vtkTransformPolyDataFilter> transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform_filter->SetInputData(data);
    transform_filter->SetTransform(landmark_transform);
    transform_filter->Update(); 

    vtkMatrix4x4* mat = landmark_transform->GetMatrix();
    std::cout << *mat << std::endl;
    return transform_filter->GetOutput();
}

void Manipulator::VisualizeResults(vtkSmartPointer<vtkOBJReader> reader)
{
    vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();

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
    _file_path = file;
}

std::string Manipulator::GetObjFilePath()
{
    return _file_path;
}

cv::Vec3d Manipulator::BarycentricCoord(
    cv::Vec3d nXYZ, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC)
{
    auto v0 = nB - nA;
    auto v1 = nC - nA;
    auto v2 = nXYZ - nA;
    auto dot00 = v0.dot(v0);
    auto dot01 = v0.dot(v1);
    auto dot11 = v1.dot(v1);
    auto dot20 = v2.dot(v0);
    auto dot21 = v2.dot(v1);
    auto invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    cv::Vec3d output;
    output[1] = (dot11 * dot20 - dot01 * dot21) * invDenom;
    output[2] = (dot00 * dot21 - dot01 * dot20) * invDenom;
    output[0] = 1.0 - output[1] - output[2];
    return output;
}
// Find Cartesian coordinates of point in triangle given barycentric coordinate
cv::Vec3d Manipulator::CartesianCoord(
    cv::Vec3d nUVW, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC)
{
    return nUVW[0] * nA + nUVW[1] * nB + nUVW[2] * nC;
}
