#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace rt
{

class ImageStitcher
{
public:
    ImageStitcher() = default;

    void setImages(std::vector<cv::Mat> i) { imgs_ = i; }

    cv::Mat compute();

private:
    std::vector<cv::Mat> imgs_;
};

}  // namespace rt