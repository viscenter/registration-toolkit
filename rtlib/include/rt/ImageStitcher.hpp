#pragma once

#include <vector>

#include <opencv2/core.hpp>

#include <opencv2/stitching/detail/matchers.hpp>
#include "opencv2/stitching/warpers.hpp"
#include <opencv2/stitching.hpp>

namespace rt
{

class ImageStitcher
{
public:
    ImageStitcher() = default;

    //void setImages(cv::InputArrayOfArrays i) { i.getUMatVector(imgs_); }

    cv::Mat compute(cv::InputArrayOfArrays i);

private:
    //std::vector<cv::UMat> imgs_;
    //std::vector<cv::UMat> masks_;
};

}  // namespace rt