//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <opencv2/core.hpp>
#include <boost/filesystem.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/types/ITKMesh.hpp"

namespace rt {
    /**
     * @class Mesh
     * @brief Represent the 3D mesh reconstruction of an artifact for the
     * registration pipeline
     * @ingroup data
     */
    class Mesh : public Data
    {
    public:
        /** Constructor */
        Mesh(const boost::filesystem::path& path){}

        /** @brief Return the fixed image (texture) of the input data */
        cv::Mat getImage() { return cvFixedImage_; }

        /** @brief Return the fixed image of the input data */
        rt::Image8UC3::Pointer getFixedImage() { return fixedImage_; }

        /** @brief Return the 3-D mesh of the input data */
        rt::ITKMesh::Pointer getMesh() { return origMesh_; }

    private:
        /** Fixed texture image of the mesh */
        cv::Mat cvFixedImage_;

        /** Fixed image */
        rt::Image8UC3::Pointer fixedImage_;

        /** 3D mesh of the input data */
        rt::ITKMesh::Pointer origMesh_;
    };
}
