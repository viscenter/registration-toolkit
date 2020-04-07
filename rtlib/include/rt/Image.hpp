//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "rt/Data.hpp"


namespace rt {
    /**
     * @class Image
     * @brief Represent a simple 2D image for the registration pipeline
     * @ingroup data
     */
    class Image : public Data
    {
    public:
        /** Constructor */
        explicit Image(const boost::filesystem::path& path);

        /** @brief Return a single image */
        cv::Mat getImage(int idx = 0) { return img_; }

        /** @brief Return the number of images we have. Only 1 because Image object represents 1 image */
        int getNumImages() { return 1; }

        /** @brief Return whether or not this object has a mesh */
        bool hasMesh() { return false; }

        /** @brief Return a single mesh from the input data */
        rt::ITKMesh::Pointer getMesh(int idx = 0) { return nullptr; }

        /** @brief Return a single image from the input data */
        int getNumMeshes() { return 0; }

    private:
        /** Image */
        cv::Mat img_;
    };
}
