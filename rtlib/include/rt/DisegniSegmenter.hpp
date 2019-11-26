#pragma once

// C++ Standard Libraries
#include <string>
#include <utility>
#include <vector>

// External Libraries
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
// Image Blur Library
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

/*
 * This algorithm takes an input Mat image and inserts it into a private class
 * member. This private class member is ran through a watershed algorithm where
 * it's differing color intensities are used to segment the image. After being
 * ran through the watershed algorithm, the image is segmented into different
 * pixel subregions that can be manipulated and displayed through various
 * methods. Next the watersheded image is ran through a split and merge
 * function. The split and merge function took advantage of the differing pixel
 * subregions to insert them into each of their own Mat image. Finally the split
 * Mat images are inserted into a vector and returned as a vector. This vector
 * being the output of the algorithm.
 */
class DisegniSegmenter
{

public:
    void setInputImage(cv::Mat i);
    std::vector<cv::Mat> compute();

private:
    cv::Mat inputImage_;
    std::vector<cv::Mat> resultImages_;

    cv::Mat watershed_image_();
    std::vector<cv::Mat> split_merge_(cv::Mat watershededImage);
};
