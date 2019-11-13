//
// Created by Sammy Stampley on 10/3/19.
//

#pragma once

//C++ Standard Libraries
#include <vector>
#include <utility>
#include <string>

//External Libraries
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
//Image Blur Library
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui_c.h>

//DRI Libraries








using namespace cv;
using namespace std;

class SepDisegni {

public:
    SepDisegni();
    const void SetImage(Mat input);
    const vector<Mat> GetFragmentedImage();
    vector<Mat> Compute();


private:
    Mat image_;
    Mat IntensityImage_;
    Mat watershededImage_;
    vector<Mat> fragmentedImage_;

    Mat watershed_image();
    vector<Mat> split_merge(Mat watershededImage);
};





