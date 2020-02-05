#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace rt
{
// clang-format off
/**
 * @brief Split a single image of multiple disegni into multiple images of
 * single disegni
 *
 * This class makes use of OpenCV's watershed algorithm to isolate individual
 * disegni from a single image. This implementation is heavily based off of the
 * OpenCV tutorial
 * <a href="https://docs.opencv.org/3.4/d2/dbd/tutorial_distance_transform.html">"Image Segmentation with Distance Transform and Watershed Algorithm"</a>.
 */
// clang-format on
class DisegniSegmenter
{
public:
    /** @brief Set the input disegni image */
    void setInputImage(const cv::Mat& i);

    /** @brief Convert white pixels to black pixels prior to segmentation */
    void setPreprocessWhiteToBlack(bool b);
    /** @brief Laplacian sharpen input image prior to segmentation */
    void setPreprocessSharpen(bool b);
    /** @brief Median blur input image prior to segmentation */
    void setPreprocessBlur(bool b);

    /** @brief Compute disegni segmentation */
    std::vector<cv::Mat> compute();

    /**
     * @brief Get labeled image
     *
     * @param colored If `true` (default), converts the labeled image to
     * an RGB image (CV_8UC3). Otherwise, returns pixel labels (CV_32SC1).
     */
    cv::Mat getLabeledImage(bool colored = true);

    /** @brief Get segmented disegni images */
    std::vector<cv::Mat> getOutputImages() const;

private:
    /** Source image */
    cv::Mat input_;
    /** Labeled image */
    cv::Mat labeled_;
    /** Segmented subregions */
    std::vector<cv::Mat> results_;

    /** Preprocessing: White-to-black */
    bool whiteToBlack_{false};
    /** Preprocessing: Sharpen */
    bool sharpen_{false};
    /** Preprocessing: Blur */
    bool blur_{false};

    /** Preprocessing */
    cv::Mat preprocess_();

    /** Run watershed on image */
    static cv::Mat watershed_image_(const cv::Mat& input);

    /** Use labeled image to convert input into several images */
    static std::vector<cv::Mat> split_labeled_image_(
        const cv::Mat& input, const cv::Mat& labeled);

    //Otsu Image Segmentation
    cv::Mat otsu_segmentation_(const cv::Mat& input, int thresholdNumber);
};
}  // namespace rt