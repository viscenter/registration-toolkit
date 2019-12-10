#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/ImageStitcher.hpp"


namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto divide_images = false;

int main(int argc, char* argv[])
{
    /*po::options_description required("Stitching Options");
    required.add_options()
            ("help,h", "Show this message")
            ("images,imgs", po::value<std::vector<std::string>>()->required(), "Images to be stitched")
            ("output-file,o", po::value<std::string>()->required(),
             "Output image")
            ("input-ldm,ldm", po::value<std::string>(), "User generated landmarks file");*/
            /*("output-ldm", po::value<std::string>(),
             "Output file path for the generated landmarks file")
            ("output-tfm,t", po::value<std::string>(),
             "Output file path for the generated transform file");*/
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << "[output] {[input] ...}"
                  << std::endl;
        return EXIT_FAILURE;
    }

    fs::path output = argv[1];
    std::vector<cv::Mat> imgs;
    // Uncomment when done testing
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

    /*for(int i = 2; i < argc; i++){
        cv::Mat img = cv::imread(argv[i]);
        imgs.push_back(img);
    }
    std::vector<std::pair<float, float> >img1_points;
    std::vector<std::pair<float, float> >img2_points;
    std::ifstream landmarkFile;
    landmarkFile.open(argv[4]);
    if(!landmarkFile.is_open()){
        return 1;
    }
    std::pair<float, float> point1;
    std::pair<float, float> point2;
    landmarkFile >> point1.first;
    while(!landmarkFile.eof()) {
        landmarkFile >> point1.second;
        img1_points.push_back(point1);
        landmarkFile >> point2.first;
        landmarkFile >> point2.second;
        img2_points.push_back(point2);
        landmarkFile >> point1.first;
    }
    landmarkFile.close();*/

    rt::ImageStitcher stitcher;
    stitcher.setImages(imgs);
    //stitcher.setLandmarks(point1, point2);
    auto res = stitcher.compute();

    cv::imwrite(output.string(), res);
}