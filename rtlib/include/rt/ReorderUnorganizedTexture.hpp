#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vc/core/types/UVMap.hpp>

namespace rt
{

class ReorderUnorganizedTexture
{
public:
    constexpr static double DEFAULT_SAMPLE_RATE = 0.1;

    // Constructors
    ReorderUnorganizedTexture() : sampleRate_(DEFAULT_SAMPLE_RATE){}
    ReorderUnorganizedTexture(
        const vtkSmartPointer<vtkPolyData>& mesh,
        const volcart::UVMap& uv,
        const cv::Mat& img)
        : inputMesh_(mesh)
        , inputUV_(uv)
        , inputTexture_(img)
        , sampleRate_(DEFAULT_SAMPLE_RATE){}

    // Inputs
    void setMesh(const vtkSmartPointer<vtkPolyData>& mesh)
    {
        inputMesh_ = mesh;
    }
    void setUVMap(const volcart::UVMap& uv) { inputUV_ = uv; }
    void setTextureMat(const cv::Mat& img) { inputTexture_ = img; }
    void setSampleRate(float s) { sampleRate_ = s; }

    // Process
    cv::Mat compute();

    // Get output
    volcart::UVMap getUVMap() { return outputUV_; }
    cv::Mat getTextureMat() { return outputTexture_; }

private:
    void align_mesh_();
    void create_texture_();
    void create_uv_();

    cv::Vec3d barycentric_coord_(
        cv::Vec3d nXYZ, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC);

    cv::Vec3d cartesian_coord_(
        cv::Vec3d nUVW, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC);

    // Input
    vtkSmartPointer<vtkPolyData> inputMesh_;
    volcart::UVMap inputUV_;
    cv::Mat inputTexture_;

    // Intermediate and parameters
    double sampleRate_;
    vtkSmartPointer<vtkPolyData> alignedMesh_;
    double alignedMeshMaxX_;
    double alignedMeshMaxY_;

    // Output
    volcart::UVMap outputUV_;
    cv::Mat outputTexture_;
};
}
