//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace rt
{
/**
 * @class SpatialObject
 * @brief Abstract data structure class to represents the input and
 * output for the registration pipeline
 * @ingroup data
 */
class SpatialObject
{

public:
    using Pointer = std::shared_ptr<SpatialObject>;

    /** @brief Load the data type from the given file. This path should be the
     * root path of a SpatialObject object from the SpatialObject subdirectory
     * of a Registration Package*/
    static Pointer Load(const boost::filesystem::path& path);

    /** @brief Return a single image from the input data */
    virtual cv::Mat getImage(int idx = 0) = 0;

    /** @brief Return a single image from the input data */
    virtual int getNumImages() = 0;

    /** @brief Return whether or not this object has a mesh */
    virtual bool hasMesh() = 0;

    /** @brief Return a single mesh from the input data */
    virtual rt::ITKMesh::Pointer getMesh(int idx = 0) = 0;

    /** @brief Return a single image from the input data */
    virtual int getNumMeshes() = 0;

    /** @brief Return the UV Map from the input data */
    virtual UVMap getUVMap() = 0;

protected:
    SpatialObject() = default;

    /** Constructor */
    SpatialObject(const boost::filesystem::path& path) {}
};
}  // namespace rt
