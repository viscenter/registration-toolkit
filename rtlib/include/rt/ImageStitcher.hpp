#pragma once

#include <vector>
#include <iostream>

#include <opencv2/stitching.hpp>

namespace rt
{

class ImageStitcher
{
public:
    struct LandmarkPair{
        int srcIdx;
        int dstIdx;
        std::vector<std::pair<float, float> > srcLdms;
        std::vector<std::pair<float, float> > dstLdms;
    };

    ImageStitcher() = default;

    // Sets the images to be stitched
    void setImages(cv::InputArrayOfArrays i) { i.getUMatVector(imgs_); }

    // Stitches the images together
    cv::Mat compute();

    // Sets the user generated landmarks
    void setLandmarks(std::vector<LandmarkPair> ldms);

    // Sets the bool to whether or not landmarks should be generated
    void setGenerateLandmarks(bool generate);

    void setOption(int option);

    void printFeatures(std::string filePath);

    void printMatches(std::string filePath);

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
    std::vector<cv::detail::ImageFeatures> allFeatures_;
    std::vector<cv::detail::MatchesInfo> allPairwiseMatches_;
    bool generateLandmarks_ = true;
    int option_ = 1;
    std::vector<LandmarkPair> landmarks_;

    void insert_user_matches_(const LandmarkPair& ldmPair);

    // Automatically finds features for the images
    std::vector<cv::detail::ImageFeatures> find_features_(double workScale);

    // Automatically finds the matches between features for the images
    void find_matches_(double confThresh, std::vector<cv::UMat>& seamEstImgs, std::vector<cv::Size>& fullImgSizes);

    // Estimates the camera parameters that the photos were taken with
    std::vector<cv::detail::CameraParams> estimate_camera_params_(double confThresh, float& warpedImageScale);

    // Stitches together the images and returns the larger image
    cv::Mat compose_pano_(double seamWorkAspect, float warpedImageScale, double workScale, std::vector<cv::UMat>& seamEstImgs, std::vector<cv::detail::CameraParams>& cameras, std::vector<cv::Size>& fullImgSizes);

    int search_img_index_(int imgIndex);

    void compute_homography_(cv::detail::MatchesInfo& matchesInfo);

    int search_matches_(int src, int dst);

    void create_matches_();

};

}  // namespace rt