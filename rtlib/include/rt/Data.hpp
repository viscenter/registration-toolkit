//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <string>

#include <opencv2/core.hpp>



namespace Data {
    class Data
    {

    public:
        static Data* Load(std::string path);
        virtual cv::Mat getImage(int idx);

    protected:
        Data() = default;
        Data(std::string path){}; //Constructor

    };


    class Image : public Data
    {
    public:
        Image(std::string path){ /* Do something to load */};
        cv::Mat getImage(int idx = 0){ return img_; }; //Return this image
    private:
        cv::Mat img_;
    };



    class ENVI : public Data
    {
    public:
        ENVI(std::string path){ /* Do something to load */ };
        cv::Mat getImage(int idx){ return bands_[idx]; };
    private:
        std::vector<cv::Mat> bands_;
    };
}
