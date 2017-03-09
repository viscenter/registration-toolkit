#pragma once

#include <opencv2/core.hpp>

namespace rt
{
/** @brief Pair of matched positions
 *
 * The first element in the pair is a position in the fixed image. The
 * second element is from the moving image.
 */
using LandmarkPair = std::pair<cv::Point2f, cv::Point2f>;

/**
 * @class LandmarkDetector
 * @brief Automatically generate landmark pairs between a two images
 * @author Ali Bertelsman
 *
 * Uses BRISK feature descriptors to generate pairs of matching key points
 * between two images.
 *
 * @ingroup rtlib
 */
class LandmarkDetector
{
public:
    /** @brief Set the fixed image */
    void setFixedImage(const cv::Mat& img) { fixedImg_ = img; }
    /** @brief Set the moving image */
    void setMovingImage(const cv::Mat& img) { movingImg_ = img; }

    /** @brief Compute keypoint matches between the fixed and moving images
     *
     * Returns up to the number of matches specified by numMatches. If
     * numMatches = -1 (default), then all matches are returned.
     */
    std::vector<LandmarkPair> compute(int numMatches = -1);

    /** @brief Get the computed matches
     *
     * Returns up to the number of matches specified by numMatches. If
     * numMatches = -1, then all matches are returned.
     */
    std::vector<LandmarkPair> getLandmarkPairs(int numMatches);

private:
    /** Fixed image */
    cv::Mat fixedImg_;
    /** Moving image */
    cv::Mat movingImg_;
    /** Matched pairs */
    std::vector<LandmarkPair> output_;
};
}
