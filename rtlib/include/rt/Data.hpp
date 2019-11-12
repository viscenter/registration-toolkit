//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <opencv2/core.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/types/ITKMesh.hpp"



namespace Data {
    class Data
    {
        using Pointer = std::shared_ptr<Data>; //DOES THIS GO HERE?

    public:
        static Pointer Load(std::string path);
        virtual cv::Mat getImage(int idx = 0) = 0;

    protected:
        Data() = default;
        Data(std::string path) {} //Constructor

    };


    class Image : public Data
    {
    public:
        Image(std::string path){ /* Do something to load */}
        cv::Mat getImage() { return img_; } //Return this image
    private:
        cv::Mat img_;
    };



    class ENVI : public Data
    {
    public:
        ENVI(std::string path){ /* Do something to load */ }
        cv::Mat getImage(int idx = 0) { return bands_[idx]; }
    private:
        std::vector<cv::Mat> bands_;
    };


    class Mesh : public Data
    {
    public:
        Mesh(std::string path){ /* Do something to load */ }
        cv::Mat getCVImage() { return cvFixedImage_; }
        rt::Image8UC3::Pointer getFixedImage() { return fixedImage_; }
        rt::ITKMesh::Pointer getMesh() { return origMesh_; }
    private:
        cv::Mat cvFixedImage_;
        rt::Image8UC3::Pointer fixedImage_;
        rt::ITKMesh::Pointer origMesh_;
    };
}
