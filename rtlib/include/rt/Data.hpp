//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/types/ITKMesh.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;


namespace Data {
    /**
     * @class Data
     * @brief Abstract data structure class to represents the input and
     * output for the registration pipeline
     * @ingroup data
     */
    class Data
    {
        using Pointer = std::shared_ptr<Data>; //DOES THIS GO HERE?

    public:
        /** @brief Load the data type from the given file */
        static Pointer Load(const fs::path& path);

        /** @brief Return a single image from the input data */
        virtual cv::Mat getImage(int idx = 0) = 0;

    protected:
        Data() = default;

        /** Constructor */
        Data(std::string path) {}

    };


    /**
     * @class Image
     * @brief Represent a simple 2D image for the registration pipeline
     * @ingroup data
     */
    class Image : public Data
    {
    public:
        /** Constructor */
        Image(const fs::path& path){ /* Do something to load */}

        /** @brief Return a single image */
        cv::Mat getImage() { return img_; }
    private:
        /** Image */
        cv::Mat img_;
    };


    /**
     * @class EVNI
     * @brief Represent an ENVI hyperspectral image for the registration pipeline
     * @ingroup data
     */
    class ENVI : public Data
    {
    public:
        /** Constructor */
        ENVI(const fs::path& path){ /* Do something to load */ }
        //cv::Mat getImage(int idx = 0) { return bands_[idx]; } //Could use this if we read bands into vector. But would also need functions to load into vector, and access vector.

        /** @brief Return a single image from the input data */
        cv::Mat getImage(int idx = 0) {}
    private:
        //std::vector<cv::Mat> bands_; //Could use to store bands we have read in memory?
        /** ENVI object */
        et::ENVI envi_;
    };


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
        Mesh(const fs::path& path){ /* Do something to load */ }

        /** @brief Return the fixed image (texture) of the input data */
        cv::Mat getCVImage() { return cvFixedImage_; }

        /** @brief Return the fixed image of the input data */
        rt::Image8UC3::Pointer getFixedImage() { return fixedImage_; }

        /** @brief Return the 3D mesh */
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
