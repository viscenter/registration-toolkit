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

    void setImages(cv::InputArrayOfArrays i) { i.getUMatVector(imgs_); }

    cv::Mat compute();

    void setLandmarks(std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2);

    void setGenerateLandmarks(bool generate){ generateLandmarks_ = generate; }

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
    std::vector<std::pair<float, float> > features1_;
    std::vector<std::pair<float, float> > features2_;
    bool generateLandmarks_ = true;

    void findFeaturesAndMatches(double work_scale, double conf_thresh_, double seam_work_aspect_, std::vector<cv::Size>& full_img_sizes_,
                                std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::ImageFeatures>& features_, std::vector<cv::detail::MatchesInfo>& pairwise_matches_);

    std::vector<cv::detail::CameraParams> estimateCameraParams(double conf_thresh_, float& warped_image_scale_, std::vector<cv::detail::ImageFeatures>& features_,
                                                               std::vector<cv::detail::MatchesInfo>& pairwise_matches_);

    cv::Mat composePano(double seam_work_aspect_, float warped_image_scale_, double work_scale_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::CameraParams>& cameras_, std::vector<cv::Size>& full_img_sizes_);

};

}  // namespace rt