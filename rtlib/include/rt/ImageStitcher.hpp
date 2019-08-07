#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace rt
{

class ImageStitcher
{
public:
    ImageStitcher() = default;

    void setImages(cv::InputArrayOfArrays i) { i.getUMatVector(imgs_); }

    cv::Mat compute();

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
};

}  // namespace rt