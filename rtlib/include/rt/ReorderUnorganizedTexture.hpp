#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include "rt/types/UVMap.hpp"

namespace rt
{

/**
 * @class ReorderUnorganizedTexture
 * @brief Reorder an unorganized texture image into an organized texture using
 * 3D mapping information
 *
 * This algorithm reorganizes embedded color information from an input mesh
 * into a new, organized texture image. The effect is similar to taking a
 * snapshot from "above" the input mesh: faces that are adjacent in 3D are also
 * adjacent in the new 2D texture image.
 *
 * This algorithm assumes the input mesh (and therefore the resulting
 * reorganized texture) is roughly planar. The mesh is realigned such that its
 * shortest dimension is perpendicular to the XY plane. The algorithm samples
 * the XY plane into an image, creating one pixel every user-specified interval.
 * A Z-axis aligned ray through the pixel is used to calculate an intersection
 * and correspondence between the mesh and the new image. The color of the
 * intersected point is bilinearly interpolated from the original texture image
 * and placed into the pixel. A UV map is also generated that maps the original
 * input mesh to the new texture image.
 *
 * @ingroup rtlib
 */
class ReorderUnorganizedTexture
{
public:
    /** Default distance (in mesh units) at which to sample the XY plane into
     * image */
    constexpr static double DEFAULT_SAMPLE_RATE = 0.1;

    /**@{*/
    /** Default */
    ReorderUnorganizedTexture() : sampleRate_(DEFAULT_SAMPLE_RATE){}
    /** @brief Construct with all input parameters */
    ReorderUnorganizedTexture(
        const vtkSmartPointer<vtkPolyData>& mesh,
        const UVMap& uv,
        const cv::Mat& img,
        double rate = DEFAULT_SAMPLE_RATE)
        : inputMesh_(mesh), inputUV_(uv), inputTexture_(img), sampleRate_(rate)
    {
    }
    /**@}*/

    /**@{*/
    /** @brief Set the input mesh */
    void setMesh(const vtkSmartPointer<vtkPolyData>& mesh)
    {
        inputMesh_ = mesh;
    }
    /** @brief Set the input UV map for the mesh */
    void setUVMap(const UVMap& uv) { inputUV_ = uv; }
    /** @brief Set the input, unorganized texture image */
    void setTextureMat(const cv::Mat& img) { inputTexture_ = img; }
    /** @brief Set the rate (in mesh units) at which to sample XY plane into an
     * image
     */
    void setSampleRate(double s) { sampleRate_ = s; }
    /**@}*/

    /**@{*/
    /** @brief Generate the new texture image and UV map
     *
     * @return The new texture image
     */
    cv::Mat compute();
    /**@}*/

    /**@{*/
    /** @brief Get the output UV map */
    UVMap getUVMap() { return outputUV_; }
    /** @brief Get the output texture image */
    cv::Mat getTextureMat() { return outputTexture_; }
    /**@}*/

private:
    /** @brief Resample the input image into the organized texture */
    void create_texture_();
    /** @brief Generate a new UV map relating the input mesh to the organized
     * texture
     */
    void create_uv_();

    /** Generate the barycentric coordinate of cartesian coordinate XYZ in
     * triangle ABC
     *
     * Note: Barycentric coordinates are relative to the first vertex. The
     * barycentric coordinate of XYZ in ABC is different from the barycentric
     * coordinate of XYZ in BAC.
     * */
    cv::Vec3d barycentric_coord_(
        cv::Vec3d XYZ, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C);

    /** Generate the cartesian coordinate of barycentric coordinate UVW in
     * triangle ABC */
    cv::Vec3d cartesian_coord_(
        cv::Vec3d UVW, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C);

    /** Input mesh */
    vtkSmartPointer<vtkPolyData> inputMesh_;
    /** Input UV map */
    UVMap inputUV_;
    /** Input texture image */
    cv::Mat inputTexture_;

    /** XY plane sample rate (in mesh units) */
    double sampleRate_;

    /**
     *
     */
    cv::Vec3d origin_;
    cv::Vec3d xAxis_;
    cv::Vec3d yAxis_;
    cv::Vec3d zAxis_;

    /** Output UV map */
    UVMap outputUV_;
    /** Output texture image */
    cv::Mat outputTexture_;
};
}
