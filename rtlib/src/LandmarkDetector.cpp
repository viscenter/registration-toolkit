#include "rt/LandmarkDetector.hpp"

#include <algorithm>
#include <exception>

#include <opencv2/features2d.hpp>
#include <opencv2/stitching/detail/matchers.hpp>

#include "rt/util/ImageConversion.hpp"

using namespace rt;

void LandmarkDetector::setFixedImage(const cv::Mat& img) { fixedImg_ = img; }
void LandmarkDetector::setFixedMask(const cv::Mat& img) { fixedMask_ = img; }
void LandmarkDetector::setMovingImage(const cv::Mat& img) { movingImg_ = img; }
void LandmarkDetector::setMovingMask(const cv::Mat& img) { movingMask_ = img; }
void LandmarkDetector::setMatchRatio(float r) { nnMatchRatio_ = r; }

// Compute the matches
std::vector<rt::LandmarkPair> LandmarkDetector::compute()
{
    // Make sure we have the images
    if (fixedImg_.data == nullptr || movingImg_.data == nullptr) {
        throw std::runtime_error("Missing image(s)");
    }

    // Clear the output vector
    output_.clear();

    // Detect key points and compute their descriptors
    auto featureDetector = cv::AKAZE::create();
    std::vector<cv::detail::ImageFeatures> features(2);
    std::vector<cv::Mat> images{fixedImg_, movingImg_};
    cv::detail::computeImageFeatures(featureDetector, images, features);

    // Safety check
    if (features[0].keypoints.size() < 2 or features[1].keypoints.size() < 2) {
        // Not enough features to match
        return output_;
    }

    // Match keypoints
    cv::detail::BestOf2NearestMatcher matcher(false, nnMatchRatio_);
    std::vector<cv::detail::MatchesInfo> matches;
    matcher(features, matches);
    matcher.collectGarbage();

    // Get only Fixed <-> Moving matches
    cv::detail::MatchesInfo fixedToMoving;
    cv::detail::MatchesInfo movingToFixed;
    for (const auto& m : matches) {
        if (m.src_img_idx == 0 and m.dst_img_idx == 1) {
            fixedToMoving = m;
        } else if (m.src_img_idx == 1 and m.dst_img_idx == 0) {
            movingToFixed = m;
        }
    }

    // Add all Fixed -> Moving inliers
    for (size_t idx = 0; idx < fixedToMoving.matches.size(); idx++) {
        size_t numInliers = fixedToMoving.num_inliers;
        if (idx < numInliers and fixedToMoving.inliers_mask[idx] == 0) {
            continue;
        }

        const auto& m = fixedToMoving.matches[idx];
        auto fixPt = features[0].keypoints[m.queryIdx].pt;
        auto movPt = features[1].keypoints[m.trainIdx].pt;
        output_.emplace_back(fixPt, movPt);
    }

    // Add Moving -> Fixed inliers not already in output
    for (size_t idx = 0; idx < movingToFixed.matches.size(); idx++) {
        size_t numInliers = movingToFixed.num_inliers;
        if (idx < numInliers and movingToFixed.inliers_mask[idx] == 0) {
            continue;
        }

        const auto& m = movingToFixed.matches[idx];
        auto fixPt = features[0].keypoints[m.trainIdx].pt;
        auto movPt = features[1].keypoints[m.queryIdx].pt;
        if (std::find(
                output_.begin(), output_.end(), LandmarkPair{fixPt, movPt}) ==
            output_.end()) {
            output_.emplace_back(fixPt, movPt);
        }
    }
    return output_;
}

// Return previously computed matches
std::vector<rt::LandmarkPair> LandmarkDetector::getLandmarkPairs()
{
    return output_;
}

LandmarkContainer LandmarkDetector::getFixedLandmarks() const
{
    LandmarkContainer res;
    Landmark l;
    for (const auto& p : output_) {
        l[0] = p.first.x;
        l[1] = p.first.y;
        res.push_back(l);
    }
    return res;
}

LandmarkContainer LandmarkDetector::getMovingLandmarks() const
{
    LandmarkContainer res;
    Landmark l;
    for (const auto& p : output_) {
        l[0] = p.second.x;
        l[1] = p.second.y;
        res.push_back(l);
    }
    return res;
}