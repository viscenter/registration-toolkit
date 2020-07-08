//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Envi.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
using namespace rt;

ENVI::ENVI(const fs::path& path) : envi_{path} {}

cv::Mat ENVI::getImage(size_t idx)
{
    // Get image of particular spectral band
    cv::Mat img = envi_.getBand(idx);

    return img;
}

int ENVI::getNumImages()
{
    return envi_.getWavelengths().size();
}
