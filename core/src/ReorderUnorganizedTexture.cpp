#include "rt/ReorderUnorganizedTexture.hpp"

#include <array>

#include <bvh/bvh.hpp>
#include <bvh/primitive_intersectors.hpp>
#include <bvh/ray.hpp>
#include <bvh/single_ray_traverser.hpp>
#include <bvh/sweep_sah_builder.hpp>
#include <bvh/triangle.hpp>
#include <bvh/vector.hpp>
#include <opencv2/imgproc.hpp>
#include <vtkOBBTree.h>

#include "rt/types/ITK2VTK.hpp"

using Scalar = double;
using Vector3 = bvh::Vector3<Scalar>;
using Triangle = bvh::Triangle<Scalar>;
using Ray = bvh::Ray<Scalar>;
using Bvh = bvh::Bvh<Scalar>;
using Intersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle>;
using Traverser = bvh::SingleRayTraverser<Bvh>;

using namespace rt;

// Generate the cartesian coordinate of barycentric coordinate uvw in tri abc
static inline auto BaryToXYZ(
    const cv::Vec3d& uvw,
    const cv::Vec3d& a,
    const cv::Vec3d& b,
    const cv::Vec3d& c) -> cv::Vec3d
{
    return uvw[0] * a + uvw[1] * b + uvw[2] * c;
}

// Get the vertices belong to a cell
template <typename CellIterator>
static inline auto GetCellVertices(
    const ITKMesh::Pointer& mesh, CellIterator& cell)
{
    std::vector<cv::Vec3d> pts;
    for (const auto& id : cell->Value()->GetPointIdsContainer()) {
        auto p = mesh->GetPoint(id);
        pts.template emplace_back(p[0], p[1], p[2]);
    }
    return pts;
}

// Check if a value is near zero
template <
    typename T,
    std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
static inline auto NearZero(T val, T eps = 1e-7) -> bool
{
    return std::abs(val) <= eps;
}

// Calculate the pixel density of the UV map
static inline auto ComputeUVDensity(
    const ITKMesh::Pointer& mesh,
    const UVMap& uv,
    double imgWidth,
    double imgHeight) -> double
{
    double density{0};
    std::size_t count{0};

    auto maxXIdx = imgWidth - 1;
    auto maxYIdx = imgHeight - 1;

    // For each face
    for (auto cell = mesh->GetCells()->Begin(); cell != mesh->GetCells()->End();
         ++cell) {

        // Get the 3D vertices
        auto pts = GetCellVertices(mesh, cell);

        // Get the UV coordinates for this face
        auto uvs = uv.getFaceUVs(cell->Index());

        // Transform UVs to image coordinates
        std::transform(
            uvs.begin(), uvs.end(), uvs.begin(),
            [maxXIdx, maxYIdx](const cv::Vec2d& p) -> cv::Vec2d {
                return {p[0] * maxXIdx, p[1] * maxYIdx};
            });

        // Update the density for each edge
        for (std::size_t idxA = 0; idxA < 3; idxA++) {
            // Next idx in the list
            auto idxB = (idxA == 2) ? 0 : idxA + 1;

            // Calculate 2D and 3D edge lengths
            auto edge3D = cv::norm(pts[idxB] - pts[idxA]);
            auto edge2D = cv::norm(uvs[idxB] - uvs[idxA]);

            // Skip if one of the lengths is zero or nan
            if (NearZero(edge3D) or std::isnan(edge3D) or NearZero(edge2D) or
                std::isnan(edge2D)) {
                continue;
            }

            // Update the density
            auto edgeDensity = edge3D / edge2D;
            count++;
            density += (edgeDensity - density) / static_cast<double>(count);
        }
    }

    return density;
}

void ReorderUnorganizedTexture::setMesh(const ITKMesh::Pointer& mesh)
{
    inputMesh_ = mesh;
}

void ReorderUnorganizedTexture::setUVMap(const UVMap& uv) { inputUV_ = uv; }

void ReorderUnorganizedTexture::setTextureMat(const cv::Mat& img)
{
    inputTexture_ = img;
}

void ReorderUnorganizedTexture::setSamplingOrigin(SamplingOrigin o)
{
    sampleOrigin_ = o;
}

auto ReorderUnorganizedTexture::samplingOrigin() const -> SamplingOrigin
{
    return sampleOrigin_;
}

void ReorderUnorganizedTexture::setSamplingMode(SamplingMode m)
{
    sampleMode_ = m;
}

auto ReorderUnorganizedTexture::samplingMode() const -> SamplingMode
{
    return sampleMode_;
}

void ReorderUnorganizedTexture::setSampleRate(double s) { sampleRate_ = s; }

auto ReorderUnorganizedTexture::sampleRate() const -> double
{
    return sampleRate_;
}

void ReorderUnorganizedTexture::setSampleDim(std::size_t d) { sampleDim_ = d; }

auto ReorderUnorganizedTexture::sampleDim() const -> std::size_t
{
    return sampleDim_;
}

void ReorderUnorganizedTexture::setUseFirstIntersection(bool b)
{
    useFirstIntersection_ = b;
}

auto ReorderUnorganizedTexture::useFirstIntersection() const -> bool
{
    return useFirstIntersection_;
}

auto ReorderUnorganizedTexture::getUVMap() -> UVMap { return outputUV_; }

auto ReorderUnorganizedTexture::getTextureMat() -> cv::Mat
{
    return outputTexture_;
}

auto ReorderUnorganizedTexture::getDepthMap() -> cv::Mat
{
    return outputDepthMap_;
}

// Compute the result
auto ReorderUnorganizedTexture::compute() -> cv::Mat
{
    create_texture_();
    create_uv_();
    return outputTexture_;
}

void ReorderUnorganizedTexture::create_texture_()
{
    // Create BVH for mesh
    std::vector<Triangle> triangles;
    for (auto cell = inputMesh_->GetCells()->Begin();
         cell != inputMesh_->GetCells()->End(); ++cell) {
        auto aIdx = cell.Value()->GetPointIdsContainer()[0];
        auto bIdx = cell.Value()->GetPointIdsContainer()[1];
        auto cIdx = cell.Value()->GetPointIdsContainer()[2];

        auto a = inputMesh_->GetPoint(aIdx);
        auto b = inputMesh_->GetPoint(bIdx);
        auto c = inputMesh_->GetPoint(cIdx);

        // Add the face to the BVH tree
        triangles.emplace_back(
            Vector3(a[0], a[1], a[2]), Vector3(b[0], b[1], b[2]),
            Vector3(c[0], c[1], c[2]));
    }
    Bvh bvh;
    bvh::SweepSahBuilder<Bvh> builder(bvh);
    auto [bboxes, centers] = bvh::compute_bounding_boxes_and_centers(
        triangles.data(), triangles.size());
    auto meshBBox =
        bvh::compute_bounding_boxes_union(bboxes.get(), triangles.size());
    builder.build(meshBBox, bboxes.get(), centers.get(), triangles.size());
    Intersector intersector(bvh, triangles.data());
    Traverser traverser(bvh);

    // Computes the OBB and returns the 3 axes relative to the box
    auto mesh = rt::ITK2VTK(inputMesh_);
    std::array<double, 3> size;
    auto obbTree = vtkSmartPointer<vtkOBBTree>::New();
    obbTree->ComputeOBB(
        mesh, origin_.val, xAxis_.val, yAxis_.val, zAxis_.val, size.data());

    int cols{-1};
    int rows{-1};
    auto xLen = cv::norm(xAxis_);
    auto yLen = cv::norm(yAxis_);

    // Calculate the sample rate
    double sampleRate{DEFAULT_SAMPLE_RATE};
    switch (sampleMode_) {
        case SamplingMode::Rate:
            sampleRate = sampleRate_;
            cols = static_cast<int>(std::ceil(xLen / sampleRate));
            rows = static_cast<int>(std::ceil(yLen / sampleRate));
            break;
        case SamplingMode::OutputWidth:
            sampleRate =
                static_cast<double>(xLen) / static_cast<double>(sampleDim_);
            cols = static_cast<int>(sampleDim_);
            rows = static_cast<int>(std::ceil(yLen / sampleRate));
            break;
        case SamplingMode::OutputHeight:
            sampleRate =
                static_cast<double>(yLen) / static_cast<double>(sampleDim_);
            cols = static_cast<int>(std::ceil(xLen / sampleRate));
            rows = static_cast<int>(sampleDim_);
            break;
        case SamplingMode::AutoUV:
            sampleRate = ComputeUVDensity(
                inputMesh_, inputUV_, inputTexture_.cols, inputTexture_.rows);
            cols = static_cast<int>(std::ceil(xLen / sampleRate));
            rows = static_cast<int>(std::ceil(yLen / sampleRate));
            break;
    }

    std::cerr << "Output size: " << cols << "x" << rows << " ";
    std::cerr << "(Sample rate: " << sampleRate << ")" << std::endl;

    // Setup the output image
    outputTexture_ = cv::Mat::zeros(rows, cols, CV_8UC3);
    outputDepthMap_ = cv::Mat::zeros(rows, cols, CV_32FC1);

    // Normalize the length
    auto normedX = cv::normalize(xAxis_);
    auto normedY = cv::normalize(yAxis_);
    auto zLen = cv::norm(zAxis_);
    if (zLen < 1.0) {
        zAxis_ = normedY.cross(normedX);
        zLen = cv::norm(zAxis_);
    } else {
        zAxis_ = cv::normalize(zAxis_);
    }

    // Change the origin and axes directions w.r.t. the sampling origin
    switch (sampleOrigin_) {
        case SamplingOrigin::TopLeft:
            // Already setup. Do nothing.
            break;
        case SamplingOrigin::TopRight:
            origin_ += xLen * normedX;
            normedX *= -1;
            break;
        case SamplingOrigin::BottomLeft:
            origin_ += yLen * normedY;
            normedY *= -1;
            break;
        case SamplingOrigin::BottomRight:
            origin_ += xLen * normedX + yLen * normedY;
            normedX *= -1;
            normedY *= -1;
            break;
    }

    for (auto v = 0; v < rows; v++) {
        for (auto u = 0; u < cols; u++) {
            // Convert pixel position to offset in mesh's XY space
            auto uOffset = u * sampleRate * normedX;
            auto vOffset = v * sampleRate * normedY;

            // Get t
            auto a0 = origin_ + uOffset + vOffset;
            auto a1 = zAxis_;
            if (not useFirstIntersection_) {
                a0 = a0 + zAxis_ * zLen;
                a1 *= -1;
            }

            // Intersect a ray with the data structure
            Vector3 start(a0[0], a0[1], a0[2]);
            Vector3 dir(a1[0], a1[1], a1[2]);
            Ray ray(start, dir, 0.0, zLen * 2);
            auto hit = traverser.traverse(ray, intersector);
            if (not hit) {
                continue;
            }

            // Assign distance to depth map
            outputDepthMap_.at<float>(v, u) =
                static_cast<float>(hit->distance());

            // Cell info
            auto cellId = hit->primitive_index;

            // Get the 2D and 3D pts
            std::vector<cv::Vec3d> uvPts;
            for (const auto& uv : inputUV_.getFaceUVs(cellId)) {
                uvPts.emplace_back(uv[0], uv[1], 0.0);
            }

            // Intersection point
            auto inter = hit->intersection;
            cv::Vec3d bCoord{inter.u, inter.v, 1 - inter.u - inter.v};

            // Get the UV position of the intersection point
            // Inexplicably, bvh barycentric coordinates are relative to the 2nd
            // pt?
            auto cPoint = BaryToXYZ(bCoord, uvPts[1], uvPts[2], uvPts[0]);

            // Convert the UV position to pixel coordinates (in orig image)
            auto x = static_cast<float>(cPoint[0] * (inputTexture_.cols - 1));
            auto y = static_cast<float>(cPoint[1] * (inputTexture_.rows - 1));

            // Bilinear interpolate color and assign to output
            cv::Mat subRect;
            cv::getRectSubPix(inputTexture_, {1, 1}, {x, y}, subRect);
            outputTexture_.at<cv::Vec3b>(v, u) = subRect.at<cv::Vec3b>(0, 0);
        }  // u
    }      // v
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

    for (size_t i = 0; i < inputMesh_->GetNumberOfPoints(); ++i) {
        // Get the point
        auto itkPt = inputMesh_->GetPoint(i);
        cv::Vec3d p{itkPt.GetDataPointer()};

        auto u = (p - origin_).dot(uVec) / uLen;
        auto v = (p - origin_).dot(vVec) / vLen;

        outputUV_.addUV({u, v});
    }

    // Add faces
    for (auto cell = inputMesh_->GetCells()->Begin();
         cell != inputMesh_->GetCells()->End(); ++cell) {
        UVMap::Face f;
        int idx{0};
        for (auto* point = cell.Value()->PointIdsBegin();
             point != cell.Value()->PointIdsEnd(); ++point) {
            f[idx++] = *point;
        }
        outputUV_.addFace(cell.Index(), f);
    }
}
