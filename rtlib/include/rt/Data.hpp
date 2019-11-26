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
    class Data
    {
        using Pointer = std::shared_ptr<Data>; //DOES THIS GO HERE?

    public:
        static Pointer Load(const fs::path& path);
        virtual cv::Mat getImage(int idx = 0) = 0;

    protected:
        Data() = default;
        Data(std::string path) {} //Constructor

    };


    class Image : public Data
    {
    public:
        Image(const fs::path& path){ /* Do something to load */}
        cv::Mat getImage() { return img_; } //Return this image
    private:
        cv::Mat img_;
    };



    class ENVI : public Data
    {
    public:
        ENVI(const fs::path& path){ /* Do something to load */ }
        //cv::Mat getImage(int idx = 0) { return bands_[idx]; } //Could use this if we read bands into vector. But would also need functions to load into vector, and access vector.
        cv::Mat getImage(int idx = 0) {}
    private:
        //std::vector<cv::Mat> bands_; //Could use to store bands we have read in memory?
        et::ENVI envi_;
    };


    class Mesh : public Data
    {
    public:
        Mesh(const fs::path& path){ /* Do something to load */ }
        cv::Mat getCVImage() { return cvFixedImage_; }
        rt::Image8UC3::Pointer getFixedImage() { return fixedImage_; }
        rt::ITKMesh::Pointer getMesh() { return origMesh_; }
    private:
        cv::Mat cvFixedImage_;
        rt::Image8UC3::Pointer fixedImage_;
        rt::ITKMesh::Pointer origMesh_;
    };
}
