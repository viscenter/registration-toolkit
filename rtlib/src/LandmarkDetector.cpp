#include "rt/LandmarkDetector.hpp"

#include <algorithm>
#include <exception>

#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>

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

    // Detect key points and compute their descriptors
    auto featureDetector = cv::AKAZE::create();
    std::vector<cv::KeyPoint> fixedKeyPts, movingKeyPts;
    cv::Mat fixedDesc, movingDesc;
    featureDetector->detectAndCompute(
        fixedImg_, fixedMask_, fixedKeyPts, fixedDesc, false);
    featureDetector->detectAndCompute(
        movingImg_, movingMask_, movingKeyPts, movingDesc, false);

    // Compute matches from descriptors
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> matches;
    matcher.knnMatch(fixedDesc, movingDesc, matches, 2);

    // Sort according to distance between descriptor matches
    std::sort(
        matches.begin(), matches.end(),
        [](const std::vector<cv::DMatch>& a, const std::vector<cv::DMatch>& b) {
            return a[0].distance < b[0].distance;
        });

    // Ratio Test
    std::vector<cv::Point2f> goodFixed, goodMoving;
    for (const auto& m : matches) {
        // Filter according to nn ratio
        if (m[0].distance < nnMatchRatio_ * m[1].distance) {
            auto fixIdx = m[0].queryIdx;
            auto movIdx = m[0].trainIdx;

            goodFixed.emplace_back(fixedKeyPts[fixIdx].pt);
            goodMoving.emplace_back(movingKeyPts[movIdx].pt);
        }
    }

    // RANSAC outlier filtering
    cv::Mat ransacMask;
    cv::findHomography(goodMoving, goodFixed, cv::RANSAC, 3, ransacMask);
    for (size_t idx = 0; idx < goodMoving.size(); idx++) {
        if (ransacMask.at<uint8_t>(idx) > 0) {
            output_.emplace_back(goodFixed[idx], goodMoving[idx]);
        }
    }

    // Return only the matches we've requested
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