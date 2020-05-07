//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "rt/SpatialObject.hpp"

namespace rt
{
/**
 * @class Mesh
 * @brief Represent the 3D mesh reconstruction of an artifact for the
 * registration pipeline
 * @ingroup data
 */
class Mesh : public SpatialObject
{
public:
    /** Constructor */
    explicit Mesh(const boost::filesystem::path& path);

    /** @brief Return the fixed image (texture) of the input data */
    cv::Mat getImage(int idx = 0) { return texture_; }

    /** @brief Return a single image from the input data */
    virtual int getNumImages() { return 1; }

    /** @brief Return whether or not this object has a mesh */
    bool hasMesh() { return mesh_ != nullptr; }

    /** @brief Return a single mesh from the input data */
    rt::ITKMesh::Pointer getMesh(int idx = 0) { return mesh_; }

    /** @brief Return a single image from the input data */
    int getNumMeshes() { return 0; }

    /** @brief Return the UV Map from the input data */
    UVMap getUVMap() { return uvMap_; }


private:
    /** 3D mesh of the input data */
    ITKMesh::Pointer mesh_;

    /** UV Map */
    UVMap uvMap_;

    /** Mesh texture image */
    cv::Mat texture_;
};
}  // namespace rt
