#include "rt/ImageStitcher.hpp"

#include <iostream>

#include <opencv2/stitching.hpp>

using namespace rt;

cv::Mat ImageStitcher::compute()
{
    cv::Mat result;
    auto stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);
    // stitcher->setExposureCompensator(cv::detail::ExposureCompensator::createDefault(cv::detail::GainCompensator::GAIN));
    stitcher->setBlender(
        cv::detail::Blender::createDefault(cv::detail::Blender::FEATHER));
    auto status = stitcher->stitch(imgs_, result);

    if (status != cv::Stitcher::OK) {
        std::cerr << "Failed to stitch." << std::endl;
    }

    return result;
}