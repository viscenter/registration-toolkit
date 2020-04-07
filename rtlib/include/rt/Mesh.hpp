//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <itkOpenCVImageBridge.h>
#include <opencv2/core.hpp>


#include "rt/Data.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/types/ITKMesh.hpp"

using OCVBridge = itk::OpenCVImageBridge;

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
        explicit Mesh(const boost::filesystem::path& path);

        /** @brief Return the fixed image (texture) of the input data */
        cv::Mat getImage(int idx = 0) { return cvFixedImage_; }

        /** @brief Return a single image from the input data */
        virtual int getNumImages() { return 1; }

        /** @brief Return whether or not this object has a mesh */
        bool hasMesh() { return origMesh_ != nullptr; }

        /** @brief Return a single mesh from the input data */
        rt::ITKMesh::Pointer getMesh(int idx = 0) { return origMesh_; }

        /** @brief Return a single image from the input data */
        int getNumMeshes() { return 0; }



        /** @brief Return the fixed image of the input data */
        rt::Image8UC3::Pointer getFixedImage() { return fixedImage_; }

    private:
        /** Fixed texture image of the mesh */
        cv::Mat cvFixedImage_;

        /** Fixed image */
        rt::Image8UC3::Pointer fixedImage_;

        /** 3D mesh of the input data */
        rt::ITKMesh::Pointer origMesh_ = nullptr;
    };
}
