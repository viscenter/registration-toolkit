//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <opencv2/core.hpp>



namespace Data {
    class Data
    {
        using Pointer = std::shared_ptr<Data>; //DOES THIS GO HERE?

    public:
        static Pointer Load(std::string path); //This may need to change return type to a smart pointer (C++11)
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
}
