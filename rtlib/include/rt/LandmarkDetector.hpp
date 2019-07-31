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
 * between two images. To create key points bounded by a region of interest,
 * set the mask for either the static or moving image.
 *
 * @ingroup rtlib
 */
class LandmarkDetector
{
public:
    /** @brief Set the fixed image */
    void setFixedImage(const cv::Mat& img) { fixedImg_ = img; }
    /** @brief Set the fixed image mask */
    void setFixedMask(const cv::Mat& img) { fixedMask_ = img; }
    /** @brief Set the moving image */
    void setMovingImage(const cv::Mat& img) { movingImg_ = img; }
    /** @brief Set the fixed image mask */
    void setMovingMask(const cv::Mat& img) { movingMask_ = img; }
    /** @brief Set the nearest-neighbor matching ratio */
    void setMatchRatio(float r) { nnMatchRatio_ = r; }

    /** @brief Compute key point matches between the fixed and moving images
     *
     * Returns a list of matches, sorted by strength of match. The size of the
     * returned vector is guaranteed to be less than or equal to the number
     * specified by numMatches. If numMatches = -1 (default), then all matches
     * are returned.
     */
    std::vector<LandmarkPair> compute(int numMatches = -1);

    /** @brief Get the computed matches
     *
     * Returns a list of matches, sorted by strength of match. The size of the
     * returned vector is guaranteed to be less than or equal to the number
     * specified by numMatches. If numMatches = -1, then all matches are
     * returned.
     */
    std::vector<LandmarkPair> getLandmarkPairs(int numMatches);

private:
    /** Fixed image */
    cv::Mat fixedImg_;
    /** Fixed image mask */
    cv::Mat fixedMask_;
    /** Moving image */
    cv::Mat movingImg_;
    /** Moving image mask */
    cv::Mat movingMask_;
    /** Matched pairs */
    std::vector<LandmarkPair> output_;
    /** Nearest-neighbor matching ratio */
    float nnMatchRatio_{0.8f};
};
}
