#pragma once

#include <vector>

#include <opencv2/core.hpp>

namespace rt
{
/**
 * @brief Split a single image of multiple disegni into multiple images of
 single disegni
 *
 * This class uses OpenCV's watershed algorithm to split
 *  This method is heavily based off of the OpenCv tutorial "Image Segmentation
 with Distance Transform and Watershed Algorithm" available at :
 https://docs.opencv.org/3.4/d2/dbd/tutorial_distance_transform.html
 */
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
};
}  // namespace rt