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
    auto brisk = cv::BRISK::create();
    std::vector<cv::KeyPoint> fixedKeyPts, movingKeyPts;
    cv::Mat fixedDesc, movingDesc;
    brisk->detectAndCompute(
        fixedImg_, fixedMask_, fixedKeyPts, fixedDesc, false);
    brisk->detectAndCompute(
        movingImg_, movingMask_, movingKeyPts, movingDesc, false);

    // Compute matches from descriptors
    auto matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
    std::vector<cv::DMatch> matches;
    matcher->match(fixedDesc, movingDesc, matches, cv::Mat());

    // Sort according to distance between descriptor matches
    std::sort(matches.begin(), matches.end(), [](cv::DMatch a, cv::DMatch b) {
        return a.distance < b.distance;
    });

    // Transfer to the output vector
    for (auto& m : matches) {
        auto fixIdx = m.queryIdx;
        auto movIdx = m.trainIdx;
        output_.push_back({fixedKeyPts[fixIdx].pt, movingKeyPts[movIdx].pt});
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