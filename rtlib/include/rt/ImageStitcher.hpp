#pragma once

#include <vector>
#include <iostream>

#include <opencv2/core.hpp>

#include <opencv2/stitching/detail/matchers.hpp>
#include "opencv2/stitching/warpers.hpp"
#include <opencv2/stitching.hpp>

#include <opencv2/imgcodecs.hpp>

namespace rt
{

class ImageStitcher
{
public:
    ImageStitcher() = default;

    // Sets the images to be stitched
    void setImages(cv::InputArrayOfArrays i) { i.getUMatVector(imgs_); }

    // Stitches the images together
    cv::Mat compute();

    // Sets the user generated landmarks
    // This will probably need to change so that there can be more than 2 images
    void setLandmarks(std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2);

    // Sets the bool to whether or not landmarks should be generated
    void setGenerateLandmarks(bool generate){ generateLandmarks_ = generate; }

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
    std::vector<std::pair<float, float> > features1_;
    std::vector<std::pair<float, float> > features2_;
    std::vector<cv::detail::ImageFeatures> all_features_;
    std::vector<cv::detail::MatchesInfo> all_pairwise_matches_;
    bool generateLandmarks_ = true;
    //double registr_resol_{0.6};
    //double work_scale_;
    //std::vector<cv::Size> full_img_sizes_;

    // Automatically finds features for the images
    std::vector<cv::detail::ImageFeatures> findFeatures(double& work_scale_, double seam_work_aspect_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_);

    // Automatically finds the matches between features for the images
    std::vector<cv::detail::MatchesInfo> findMatches(double conf_thresh_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_, std::vector<cv::detail::ImageFeatures>& features_);

    // Estimates the camera parameters that the photos were taken with
    std::vector<cv::detail::CameraParams> estimateCameraParams(double conf_thresh_, float& warped_image_scale_, std::vector<cv::detail::ImageFeatures>& features_,
                                                               std::vector<cv::detail::MatchesInfo>& pairwise_matches_);

    // Stitches together the images and returns the larger image
    cv::Mat composePano(double seam_work_aspect_, float warped_image_scale_, double work_scale_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::CameraParams>& cameras_, std::vector<cv::Size>& full_img_sizes_);

    void insertFeatures(std::vector<cv::detail::ImageFeatures> features);

    int searchImgIndex(int img_index);

    void PrintMatSwitch(const cv::UMat& m);

    void computeHomography(cv::detail::MatchesInfo& matches_info);

    void filterMatchedFeatures(cv::detail::ImageFeatures& features, std::vector<std::pair<float, float> >& points1, std::vector<std::pair<float, float> >& points2);

    int search(cv::detail::ImageFeatures& features, std::pair<float, float>& point);

    void reduceImgPoints(const double& work_scale, std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2);

    void calcSeamEstImages(std::vector<cv::UMat>& seam_est_imgs_);

    template <typename T>
    void PrintMat(const cv::UMat& m) {
        for(int y = 0; y < m.rows; y++) {
            for(int x = 0; x < m.cols; x++) {
                std::cout << m.getMat(cv::ACCESS_READ).at<T>(y, x) << " ";
            }
            std::cout << std::endl;
        }
    }
};

}  // namespace rt