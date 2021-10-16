#include "rt/LandmarkDetector.hpp"

#include <algorithm>
#include <exception>

#include <opencv2/features2d.hpp>

using namespace rt;

void LandmarkDetector::setFixedImage(const cv::Mat& img) { fixedImg_ = img; }
void LandmarkDetector::setFixedMask(const cv::Mat& img) { fixedMask_ = img; }
void LandmarkDetector::setMovingImage(const cv::Mat& img) { movingImg_ = img; }
void LandmarkDetector::setMovingMask(const cv::Mat& img) { movingMask_ = img; }
void LandmarkDetector::setMatchRatio(float r) { nnMatchRatio_ = r; }

// Compute the matches
auto LandmarkDetector::compute() -> std::vector<rt::LandmarkPair>
{
    // Make sure we have the images
    if (fixedImg_.empty() || movingImg_.empty()) {
        throw std::runtime_error("Missing image(s)");
    }

    // Clear the output vector
    output_.clear();

    // Detect key points and compute their descriptors
    auto featureDetector = cv::AKAZE::create();
    std::vector<cv::KeyPoint> fixedKeys;
    std::vector<cv::KeyPoint> movingKeys;
    cv::Mat fixedDesc;
    cv::Mat movingDesc;
    featureDetector->detectAndCompute(
        fixedImg_, fixedMask_, fixedKeys, fixedDesc);
    featureDetector->detectAndCompute(
        movingImg_, movingMask_, movingKeys, movingDesc);

    // Match keypoints
    auto matcher = cv::DescriptorMatcher::create(
        cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
    std::vector<std::vector<cv::DMatch>> matches;
    matcher->knnMatch(fixedDesc, movingDesc, matches, 2);

    // Filter matches
    std::vector<cv::DMatch> goodMatches;
    for (const auto& m : matches) {
        if (m[0].distance < nnMatchRatio_ * m[1].distance) {
            goodMatches.push_back(m[0]);
        }
    }

    // Convert good matches to landmark pairs
    // query = fixed, train = moving
    for (const auto& m : goodMatches) {
        // From fixed -> moving
        if (m.imgIdx == 0) {
            auto fixPt = fixedKeys[m.queryIdx].pt;
            auto movPt = movingKeys[m.trainIdx].pt;
            output_.emplace_back(fixPt, movPt);
        }

        // From moving -> fixed
        else if (m.imgIdx != 0) {
            std::cerr << "Warning: Unexpected image match index: ";
            std::cerr << m.imgIdx << std::endl;
        }
    }

    return output_;
}

// Return previously computed matches
auto LandmarkDetector::getLandmarkPairs() -> std::vector<rt::LandmarkPair>
{
    return output_;
}

auto LandmarkDetector::getFixedLandmarks() const -> LandmarkContainer
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

auto LandmarkDetector::getMovingLandmarks() const -> LandmarkContainer
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

auto LandmarkDetector::matchRatio() const -> float { return nnMatchRatio_; }
