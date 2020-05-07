//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <envitools/ENVI.hpp>
#include <opencv2/core.hpp>

#include "rt/SpatialObject.hpp"

namespace rt
{
/**
* @class ENVI
* @brief Represent an ENVI hyperspectral image for the registration pipeline
* @ingroup data
*/
class ENVI : public SpatialObject
{
public:
    /** Constructor */
    explicit ENVI(const boost::filesystem::path& path);

    /** @brief Return a single image from the input data */
    cv::Mat getImage(size_t idx = 0);

    /** @brief Return the number of images we have available */
    int getNumImages();

    /** @brief Return whether or not this object has a mesh */
    bool hasMesh() { return false; }

    /** @brief Return a single mesh from the input data */
    ITKMesh::Pointer getMesh(size_t idx = 0) { return nullptr; }

    /** @brief Return a single image from the input data */
    int getNumMeshes() { return 0; }

    /** @brief Return the UV Map from the input data */
    UVMap getUVMap() { return UVMap(); }

private:
    // std::vector<cv::Mat> bands_; //Could use to store bands we have read in
    // memory?
    /** ENVI object */
    envitools::ENVI envi_;
};
}  // namespace rt
