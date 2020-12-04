#pragma once

#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

class ImageStitcher
{
public:
    enum class LandmarkMode {
        Automatic = 0,
        Manual,
        ManualFallback,
        ManualPreMatch,
        ManualPostMatch
    };

    struct LandmarkPair{
        int srcIdx;
        int dstIdx;
        LandmarkContainer srcLdms;
        LandmarkContainer dstLdms;
    };

    // Sets the images to be stitched
    void setImages(const std::vector<cv::Mat>& i);

    // Sets the user generated landmarks
    void setLandmarks(const std::vector<LandmarkPair>& ldms);

    void setLandmarkMode(LandmarkMode option);

    // Stitches the images together
    cv::Mat compute();

private:
    std::vector<cv::Mat> input_;
    std::vector<cv::Mat> masks_;
    std::vector<LandmarkPair> landmarks_;
    LandmarkMode ldmMode_{LandmarkMode::ManualPostMatch};

    cv::Mat result_;
    cv::Ptr<cv::Feature2D> featureFinder_;
};

}  // namespace rt