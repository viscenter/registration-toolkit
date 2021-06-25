#pragma once

/** @file */

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

    /**
     * @brief Set seed points for foreground objects
     *
     * Every point in this list defines the seed for a new foreground object.
     */
    void setForegroundSeeds(const std::vector<cv::Point>& b);

    /**
     * @brief Set seed points for the background
     *
     * Every point in this list defines a seed for the background layer of the
     * input image.
     */
    void setBackgroundSeeds(const std::vector<cv::Point>& b);

    /** @brief Set the radius of the seed points, in pixels */
    void setSeedSize(int s);

    /**
     * @brief Set the number of buffer pixels around objects in the output
     * images
     *
     * Adjusts the bounding box for segmented objects to include buffer space.
     * If set to 0, sub-images will be defined by the axis-aligned bounding box
     * which includes all of the pixels for a particular label. If > 0, extra
     * space will be added to returned images. If < 0, portions of the
     * segmented object will not be included in the output image.
     *
     * Default: 10
     *
     * @param b Number of buffer pixels to add to object bounding boxes
     */
    void setBoundingBoxBuffer(int b);

    /** @brief Compute disegni segmentation */
    auto compute() -> std::vector<cv::Mat>;

    /**
     * @brief Get labeled image
     *
     * If `colored` is true (default), returns an 8bpc RGB label image where
     * each segmented object has a random color. Otherwise, returns a signed
     * 32-bit single-channel image of pixel labels.
     *
     * Signed 32-bit label interpretation:
     *   - -1 - Border between objects
     *   -  0 - Unlabeled pixel
     *   -  1 - Background object
     *   - >1 - Label value
     *
     * @param colored If `true` (default), converts the labeled image to
     * an RGB image (CV_8UC3). Otherwise, returns pixel labels (CV_32SC1).
     */
    auto getLabeledImage(bool colored = true) -> cv::Mat;

    /** @brief Get segmented disegni images */
    [[nodiscard]] auto getOutputImages() const -> std::vector<cv::Mat>;

private:
    /** Source image */
    cv::Mat input_;
    /** Labeled image */
    cv::Mat labeled_;
    /** Segmented subregions */
    std::vector<cv::Mat> results_;
    /** Foreground boundary vector */
    std::vector<cv::Point> fgSeeds_;
    /** Background boundary coordinate */
    std::vector<cv::Point> bgSeeds_;
    /** Seed size (radius) */
    int seedSize_{1};

    /** Preprocessing: White-to-black */
    bool whiteToBlack_{false};
    /** Preprocessing: Sharpen */
    bool sharpen_{false};
    /** Preprocessing: Blur */
    bool blur_{false};
    /** Buffer pixels */
    int bboxBuffer_{10};

    /** Preprocessing */
    auto preprocess_() -> cv::Mat;

    /** Run watershed on image */
    auto watershed_image_(const cv::Mat& input) -> cv::Mat;

    /** Use labeled image to convert input into several images */
    auto split_labeled_image_(const cv::Mat& input, const cv::Mat& labeled)
        -> std::vector<cv::Mat> const;
};
}  // namespace rt
