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
    // This will probably need to change so that there can be more than 2 images
    void setLandmarks(std::vector<std::string> ldmFiles);

    // Sets the bool to whether or not landmarks should be generated
    void setGenerateLandmarks(bool generate);

    void setOption(int option);

    void printFeatures(std::string file_path);

    void printMatches(std::string file_path);

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
    std::vector<cv::detail::ImageFeatures> allFeatures_;
    std::vector<cv::detail::MatchesInfo> allPairwiseMatches_;
    bool generateLandmarks_ = true;
    int option_ = 1;
    std::vector<LandmarkPair> landmarks_;

    void insert_user_matches_(const LandmarkPair& landmarks);

    // Automatically finds features for the images
    std::vector<cv::detail::ImageFeatures> find_features_(double& work_scale_);

    // Automatically finds the matches between features for the images
    void find_matches_(double conf_thresh_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_);

    // Estimates the camera parameters that the photos were taken with
    std::vector<cv::detail::CameraParams> estimate_camera_params_(double conf_thresh_, float& warped_image_scale_, std::vector<cv::detail::ImageFeatures>& features_,
                                                               std::vector<cv::detail::MatchesInfo>& pairwise_matches_);

    // Stitches together the images and returns the larger image
    cv::Mat compose_pano_(double seam_work_aspect_, float warped_image_scale_, double work_scale_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::CameraParams>& cameras_, std::vector<cv::Size>& full_img_sizes_);

    int search_img_index_(int img_index);

    void compute_homography_(cv::detail::MatchesInfo& matches_info);

    int search_matches_(int src, int dst);

    void create_matches_();

};

}  // namespace rt