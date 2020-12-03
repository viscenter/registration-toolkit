#pragma once

#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

class ImageStitcher
{
public:
    enum class LandmarkMode {
        Automatic,
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
    void setLandmarks(std::vector<LandmarkPair> ldms);

    void setLandmarkMode(LandmarkMode option);

    // Stitches the images together
    cv::Mat compute();

    void printFeatures(std::string filePath);

    void printMatches(std::string filePath);

private:
    std::vector<cv::Mat> input_;
    std::vector<cv::Mat> masks_;
    std::vector<LandmarkPair> landmarks_;
    LandmarkMode ldmMode_{LandmarkMode::ManualPostMatch};

    cv::Mat result_;

    cv::Ptr<cv::Feature2D> featureFinder_;

    // Automatically finds the matches between features for the images
    void find_matches_(double confThresh, std::vector<cv::UMat>& seamEstImgs, std::vector<cv::Size>& fullImgSizes);

    // Estimates the camera parameters that the photos were taken with
    std::vector<cv::detail::CameraParams> estimate_camera_params_(double confThresh, float& warpedImageScale);

    // Stitches together the images and returns the larger image
    cv::Mat compose_pano_(double seamWorkAspect, float warpedImageScale, double workScale, std::vector<cv::UMat>& seamEstImgs, std::vector<cv::detail::CameraParams>& cameras, std::vector<cv::Size>& fullImgSizes);

    int search_img_index_(int imgIndex);

    cv::detail::MatchesInfo compute_homography_(cv::detail::MatchesInfo matchesInfo);

    int search_matches_(int src, int dst);

    void create_matches_();

};

}  // namespace rt