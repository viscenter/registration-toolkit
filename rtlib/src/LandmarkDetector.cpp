#include "rt/LandmarkDetector.hpp"

#include <algorithm>
#include <exception>

#include <opencv2/features2d.hpp>
#include <opencv2/stitching/detail/matchers.hpp>

using namespace rt;

// Compute the matches
std::vector<rt::LandmarkPair> LandmarkDetector::compute()
{
    // Make sure we have the images
    if (fixedImg_.data == nullptr || movingImg_.data == nullptr) {
        throw std::runtime_error("Missing image(s)");
    }

    // Clear the output vector
    output_.clear();

    // UMats for OpenCL support (if available)
    cv::UMat fixedImage;
    cv::UMat movingImage;
    fixedImg_.copyTo(fixedImage);
    movingImg_.copyTo(movingImage);

    // Detect key points and compute their descriptors
    auto featureDetector = cv::AKAZE::create();
    std::vector<cv::detail::ImageFeatures> features(2);
    std::vector<cv::UMat> images{fixedImage, movingImage};
    cv::detail::computeImageFeatures(featureDetector, images, features);

    // Match keypoints
    cv::detail::BestOf2NearestMatcher matcher(true, nnMatchRatio_);
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
        if (fixedToMoving.inliers_mask[idx] == 0) {
            continue;
        }

        const auto& m = fixedToMoving.matches[idx];
        auto fixPt = features[0].keypoints[m.queryIdx].pt;
        auto movPt = features[1].keypoints[m.trainIdx].pt;
        output_.emplace_back(fixPt, movPt);
    }

    // Add Moving -> Fixed inliers not already in output
    for (size_t idx = 0; idx < movingToFixed.matches.size(); idx++) {
        if (movingToFixed.inliers_mask[idx] == 0) {
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
