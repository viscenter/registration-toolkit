#include "rt/LandmarkDetector.hpp"

#include <fstream>
#include <iostream>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace rt;

// Compute the matches
std::vector<rt::LandmarkPair> LandmarkDetector::compute(int numMatches)
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
    std::sort(matches.begin(), matches.end(), [](const std::vector<cv::DMatch>& a, const std::vector<cv::DMatch>& b) {
      return a[0].distance < b[0].distance;
    });

    // Filter and transfer to the output vector
    for(const auto& m : matches) {
        if(m[0].distance < nnMatchRatio_ * m[1].distance) {
            auto fixIdx = m[0].queryIdx;
            auto movIdx = m[0].trainIdx;
            output_.emplace_back(fixedKeyPts[fixIdx].pt, movingKeyPts[movIdx].pt);
        }
    }

    // Return only the matches we've requested
    return getLandmarkPairs(numMatches);
}

// Return previously computed matches
std::vector<rt::LandmarkPair> LandmarkDetector::getLandmarkPairs(int numMatches)
{
    // Return all
    if (numMatches == -1 || output_.empty()) {
        return output_;
    }

    // Return limited number
    int n = (numMatches > static_cast<int>(output_.size()))
                ? static_cast<int>(output_.size() - 1)
                : numMatches;
    return {std::begin(output_), std::begin(output_) + n};
}