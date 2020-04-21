#pragma once

#include <vector>
#include <iostream>

#include <opencv2/stitching.hpp>

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
    void setLandmarks(std::vector<std::string> ldmFiles);

    // Sets the bool to whether or not landmarks should be generated
    void setGenerateLandmarks(bool generate){ generate_landmarks_ = generate; }

    void setOption(int option){ option_ = option; }

    void printFeatures(std::string file_path);

    void printMatches(std::string file_path);

    struct LandmarkPair{
        int src_idx;
        int dst_idx;
        std::vector<std::pair<float, float> > src_ldms;
        std::vector<std::pair<float, float> > dst_ldms;
    };

private:
    std::vector<cv::UMat> imgs_;
    std::vector<cv::UMat> masks_;
    std::vector<cv::detail::ImageFeatures> all_features_;
    std::vector<cv::detail::MatchesInfo> all_pairwise_matches_;
    bool generate_landmarks_ = true;
    int option_ = 2;
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

    void PrintMatSwitch(const cv::UMat& m);

    void compute_homography_(cv::detail::MatchesInfo& matches_info);

    void filter_matched_features_(cv::detail::ImageFeatures& features, std::vector<std::pair<float, float> >& points1, std::vector<std::pair<float, float> >& points2);

    int search_(cv::detail::ImageFeatures& features, std::pair<float, float>& point);

    void reduce_img_points_(const double& work_scale, std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2);

    int search_matches_(int src, int dst);

    void create_matches_();

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