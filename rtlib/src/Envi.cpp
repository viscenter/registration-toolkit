//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Envi.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
using namespace rt;

rt::ENVI::ENVI(const fs::path& path) : envi_{path} {}

cv::Mat rt::ENVI::getImage(int idx)
{
    // Get image of particular spectral band
    cv::Mat img = envi_.getBand(idx);

    // envi_.closeFile();

    return img;
}