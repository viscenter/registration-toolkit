//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Image.hpp"

using namespace rt;
namespace fs = boost::filesystem;

rt::Image::Image(const fs::path& path) { img_ = cv::imread(path.string()); }


