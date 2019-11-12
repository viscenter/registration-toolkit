#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/ImageStitcher.hpp"


namespace fs = boost::filesystem;

auto divide_images = false;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << "[output] {[input] ...}"
                  << std::endl;
        return EXIT_FAILURE;
    }

    fs::path output = argv[1];
    std::vector<cv::Mat> imgs;
    for (int i = 2; i < argc; i++) {
        cv::Mat img = cv::imread(argv[i]);
        if (divide_images) {
            cv::Rect rect(0, 0, img.cols / 2, img.rows);
            imgs.push_back(img(rect).clone());
            rect.x = img.cols / 3;
            imgs.push_back(img(rect).clone());
            rect.x = img.cols / 2;
            imgs.push_back(img(rect).clone());
        } else {
            imgs.push_back(img);
        }
        imgs.push_back(img);
    }

    rt::ImageStitcher stitcher;
    //stitcher.setImages(imgs);
    auto res = stitcher.compute(imgs);

    cv::imwrite(output.string(), res);
}