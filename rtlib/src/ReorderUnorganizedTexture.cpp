#include "rt/ReorderUnorganizedTexture.hpp"

#include <opencv2/imgproc.hpp>
#include <vtkLandmarkTransform.h>
#include <vtkOBBTree.h>
#include <vtkTransformPolyDataFilter.h>

using namespace rt;

// Compute the result
cv::Mat ReorderUnorganizedTexture::compute()
{
    create_texture_();
    create_uv_();

    return outputTexture_;
}

void ReorderUnorganizedTexture::create_texture_()
{
    // Computes the OBB and returns the 3 axes relative to the box
    double size[3];
    auto obbTree = vtkSmartPointer<vtkOBBTree>::New();
    obbTree->ComputeOBB(
        inputMesh_, origin_.val, xAxis_.val, yAxis_.val, zAxis_.val, size);
    obbTree->SetDataSet(inputMesh_);
    obbTree->BuildLocator();

    // Setup the output image
    // After alignment, dimensions go from [0, dimension max], therefore
    // dimension max / sample rate == # of pixels in dimension
    int cols = static_cast<int>(std::ceil(cv::norm(xAxis_) / sampleRate_));
    int rows = static_cast<int>(std::ceil(cv::norm(yAxis_) / sampleRate_));
    outputTexture_ = cv::Mat::zeros(rows, cols, CV_8UC3);

    // Normalize the length
    auto normedX = xAxis_ / cv::norm(xAxis_);
    auto normedY = yAxis_ / cv::norm(yAxis_);

    // Loop over every pixel of the image
    auto interPts = vtkSmartPointer<vtkPoints>::New();
    auto interCells = vtkSmartPointer<vtkIdList>::New();
    for (auto v = 0; v < rows; v++) {
        for (auto u = 0; u < cols; u++) {
            // Clear the intersection point and cell ID lists
            interPts->Reset();
            interCells->Reset();

            // Convert pixel position to offset in mesh's XY space
            auto uOffset = u * sampleRate_ * normedX;
            auto vOffset = v * sampleRate_ * normedY;

            // Get t
            auto a0 = origin_ + uOffset + vOffset;

            if (cv::norm(zAxis_) < 1.0) {
                zAxis_ = normedY.cross(normedX);
            }
            auto a1 = a0 + zAxis_ * 2;

            // Calculate mesh intersection
            auto res = obbTree->IntersectWithLine(
                a0.val, a1.val, interPts, interCells);
            if (res != 0) {
                auto cid = interCells->GetNumberOfIds() - 1;
                if (useFirstInterection_) {
                    cid = 0;
                }

                // Get the three vertices of the last intersected cell
                auto cell = interCells->GetId(cid);
                auto pointIds = vtkSmartPointer<vtkIdList>::New();
                inputMesh_->GetCellPoints(cell, pointIds);

                // Make sure we only have three vertices
                assert(pointIds->GetNumberOfIds() == 3);
                auto v_id0 = pointIds->GetId(0);
                auto v_id1 = pointIds->GetId(1);
                auto v_id2 = pointIds->GetId(2);

                // Get the 3D positions of each vertex
                cv::Vec3d A{inputMesh_->GetPoint(v_id0)};
                cv::Vec3d B{inputMesh_->GetPoint(v_id1)};
                cv::Vec3d C{inputMesh_->GetPoint(v_id2)};

                // Get the 3D position of the intersection pt
                auto i_id = interPts->GetNumberOfPoints() - 1;
                cv::Vec3d alpha{interPts->GetPoint(i_id)};

                // Get the barycentric coordinate of the intersection pt
                cv::Vec3d bary_point = barycentric_coord_(alpha, A, B, C);

                // Get the UV coordinates of triangle vertices
                UVMap::Face f;
                try {
                    f = inputUV_.getFace(cell);
                } catch (const std::exception& e) {
                    continue;
                }
                auto a = inputUV_.getUV(f[0]);
                auto b = inputUV_.getUV(f[1]);
                auto c = inputUV_.getUV(f[2]);

                // Setup some temp variables
                // Drop the third dimension because UV is 2D
                A = {a[0], a[1], 0.0};
                B = {b[0], b[1], 0.0};
                C = {c[0], c[1], 0.0};

                // Get the UV position of the intersection point
                cv::Vec3d cart_point = cartesian_coord_(bary_point, A, B, C);

                // Convert the UV position to pixel coordinates (in orig image)
                auto x = static_cast<float>(
                    cart_point[0] * (inputTexture_.cols - 1));
                auto y = static_cast<float>(
                    cart_point[1] * (inputTexture_.rows - 1));

                // Bilinear interpolate color and assign to output
                cv::Mat subRect;
                cv::getRectSubPix(inputTexture_, {1, 1}, {x, y}, subRect);
                outputTexture_.at<cv::Vec3b>(v, u) =
                    subRect.at<cv::Vec3b>(0, 0);
            }
        }
    }
}

// Generate a new UV map using the aligned mesh
// This is simple after alignment u = pos.x / max.x, v = pos.y / max.y
void ReorderUnorganizedTexture::create_uv_()
{
    outputUV_ = UVMap();

    auto uLen = cv::norm(xAxis_);
    auto vLen = cv::norm(yAxis_);
    auto uVec = xAxis_ / uLen;
    auto vVec = yAxis_ / vLen;

    cv::Vec3d p;
    for (auto i = 0; i < inputMesh_->GetNumberOfPoints(); ++i) {
        // Get the point
        inputMesh_->GetPoint(i, p.val);

        auto u = (p - origin_).dot(uVec) / uLen;
        auto v = (p - origin_).dot(vVec) / vLen;

        outputUV_.addUV({u, v});
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
