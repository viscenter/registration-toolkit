#include "rt/ReorderUnorganizedTexture.hpp"

#include <vtkLandmarkTransform.h>
#include <vtkOBBTree.h>
#include <vtkTransformPolyDataFilter.h>

using namespace rt;

// Compute the result
cv::Mat ReorderUnorganizedTexture::compute()
{
    align_mesh_();
    create_texture_();
    create_uv_();

    return outputTexture_;
}

// Uses landmark registration to align the input mesh to be parallel to the XY
// plane and near the origin
void ReorderUnorganizedTexture::align_mesh_()
{
    std::cerr << "Aligning mesh..." << std::endl;
    //// Computes the OBB and returns the 3 axis relative to the box
    double corner[3], min[3], mid[3], max[3], size[3];
    auto obbTree = vtkSmartPointer<vtkOBBTree>::New();
    obbTree->ComputeOBB(inputMesh_, corner, max, mid, min, size);

    //// Create the moving landmarks
    cv::Vec3d movCorner(corner);
    cv::Vec3d movX(max), movY(mid), movZ(min);

    // Normalize the axes points and make relative to corner
    movX *= 1 / cv::norm(movX);
    movY *= 1 / cv::norm(movY);
    movZ *= 1 / cv::norm(movZ);

    movX += movCorner;
    movY += movCorner;
    movZ += movCorner;

    auto movingPts = vtkSmartPointer<vtkPoints>::New();
    movingPts->InsertNextPoint(movCorner.val);
    movingPts->InsertNextPoint(movX.val);
    movingPts->InsertNextPoint(movY.val);
    movingPts->InsertNextPoint(movZ.val);

    //// Create the static landmarks
    double stcCorner[3] = {0.0, 0.0, 0.0};
    double stcX[3] = {1.0, 0.0, 0.0};
    double stcY[3] = {0.0, 1.0, 0.0};
    double stcZ[3] = {0.0, 0.0, 1.0};

    auto fixedPts = vtkSmartPointer<vtkPoints>::New();
    fixedPts->InsertNextPoint(stcCorner);
    fixedPts->InsertNextPoint(stcX);
    fixedPts->InsertNextPoint(stcY);
    fixedPts->InsertNextPoint(stcZ);

    //// Create the affine, landmark-based transform
    auto transform = vtkSmartPointer<vtkLandmarkTransform>::New();
    transform->SetSourceLandmarks(movingPts);
    transform->SetTargetLandmarks(fixedPts);
    transform->Update();

    //// Apply the transform to the mesh
    auto filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    filter->SetInputData(inputMesh_);
    filter->SetTransform(transform);
    filter->Update();
    alignedMesh_ = filter->GetOutput();
}

void ReorderUnorganizedTexture::create_texture_()
{
    // Computes the OBB and returns the 3 axes relative to the box
    double corner[3], min[3], mid[3], max[3], size[3];
    auto obbTree = vtkSmartPointer<vtkOBBTree>::New();
    obbTree->ComputeOBB(alignedMesh_, corner, max, mid, min, size);
    obbTree->SetDataSet(alignedMesh_);
    obbTree->BuildLocator();

    // Setup the output image
    // After alignment, dimensions go from [0, dimension max], therefore
    // dimension max / sample rate == # of pixels in dimension
    alignedMeshMaxX_ = max[0];
    alignedMeshMaxY_ = mid[1];
    int cols = static_cast<int>(std::ceil(alignedMeshMaxX_ / sampleRate_));
    int rows = static_cast<int>(std::ceil(alignedMeshMaxY_ / sampleRate_));
    outputTexture_ = cv::Mat::zeros(rows, cols, CV_8UC3);

    // Loop over every pixel of the image
    auto interPts = vtkSmartPointer<vtkPoints>::New();
    auto interCells = vtkSmartPointer<vtkIdList>::New();
    for (auto j = 0; j < rows; j++) {
        for (auto i = 0; i < cols; i++) {
            double progress_ = (i + 1.0 + (cols * j)) * 100.0 / (cols * rows);
            std::cerr << "Reordering texture: " << std::to_string(progress_)
                      << "%\r" << std::flush;

            // Clear the intersection point and cell ID lists
            interPts->Reset();
            interCells->Reset();

            // Convert pixel position to position in mesh's XY space
            auto n_i = i * sampleRate_;
            auto n_j = j * sampleRate_;

            // Position at ground plane and above mesh
            double a0[3] = {n_i, n_j, 0};
            double a1[3] = {n_i, n_j, std::ceil(corner[2])};

            // Calculate mesh intersection
            auto res = obbTree->IntersectWithLine(a0, a1, interPts, interCells);
            if (res != 0) {
                // Get the three vertices of the last intersected cell
                auto cell = interCells->GetId(interCells->GetNumberOfIds() - 1);
                auto pointIds = vtkSmartPointer<vtkIdList>::New();
                alignedMesh_->GetCellPoints(cell, pointIds);

                // Make sure we only have three vertices
                assert(pointIds->GetNumberOfIds() == 3);
                auto v_id0 = pointIds->GetId(0);
                auto v_id1 = pointIds->GetId(1);
                auto v_id2 = pointIds->GetId(2);

                // Get the 3D positions of each vertex
                cv::Vec3d A{alignedMesh_->GetPoint(v_id0)};
                cv::Vec3d B{alignedMesh_->GetPoint(v_id1)};
                cv::Vec3d C{alignedMesh_->GetPoint(v_id2)};

                // Get the 3D position of the intersection pt
                auto i_id = interPts->GetNumberOfPoints() - 1;
                cv::Vec3d alpha{interPts->GetPoint(i_id)};

                // Get the barycentric coordinate of the intersection pt
                cv::Vec3d bary_point = barycentric_coord_(alpha, A, B, C);

                // Get the UV coordinates of triangle vertices
                auto a = inputUV_.get(v_id0);
                auto b = inputUV_.get(v_id1);
                auto c = inputUV_.get(v_id2);

                // Setup some temp variables
                // Drop the third dimension because UV is 2D
                A = {a[0], a[1], 0.0};
                B = {b[0], b[1], 0.0};
                C = {c[0], c[1], 0.0};

                // Get the UV position of the intersection point
                cv::Vec3d cart_point = cartesian_coord_(bary_point, A, B, C);

                // Convert the UV position to pixel coordinates (in orig image)
                float x = static_cast<float>(cart_point[0]) *
                          (inputTexture_.cols - 1);
                float y = static_cast<float>(
                    cart_point[1] * (inputTexture_.rows - 1));

                // Bilinear interpolate color and assign to output
                cv::Mat subRect;
                cv::getRectSubPix(inputTexture_, {1, 1}, {x, y}, subRect);
                outputTexture_.at<cv::Vec3b>(j, i) =
                    subRect.at<cv::Vec3b>(0, 0);
            }
        }
    }
    std::cerr << std::endl;
}

// Generate a new UV map using the aligned mesh
// This is simple after alignment u = pos.x / max.x, v = pos.y / max.y
void ReorderUnorganizedTexture::create_uv_()
{
    std::cerr << "Creating UV map..." << std::endl;
    outputUV_ = volcart::UVMap();
    double p[3];
    for (auto i = 0; i < alignedMesh_->GetNumberOfPoints(); ++i) {
        alignedMesh_->GetPoint(i, p);
        outputUV_.set(
            static_cast<size_t>(i),
            {p[0] / alignedMeshMaxX_, p[1] / alignedMeshMaxY_});
    }
}

// Calculate the barycentric coordinate of cartesian XYZ in triangle ABC
cv::Vec3d ReorderUnorganizedTexture::barycentric_coord_(
    cv::Vec3d XYZ, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C)
{
    auto v0 = B - A;
    auto v1 = C - A;
    auto v2 = XYZ - A;
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
// Calculate the cartesian coordinate of barycentric UVW in triangle ABC
cv::Vec3d ReorderUnorganizedTexture::cartesian_coord_(
    cv::Vec3d UVW, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C)
{
    return UVW[0] * A + UVW[1] * B + UVW[2] * C;
}
