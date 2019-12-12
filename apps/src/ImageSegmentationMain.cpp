#include "rt/DisegniSegmenter.hpp"

#include <iostream>
#include <boost/filesystem.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>



namespace fs = boost::filesystem;


int main(int argc, char* argv[])
{

    // TESTING PURPOSE
    //Definition of the the required amount of arguments
    const int ARGUMENTS = 2;

    if(argc != ARGUMENTS){
        std::cout << "Warning: need exactly " << ARGUMENTS-1 << " command line argument(s)." << std::endl;
        std::cout << "Usage: " << argv[0] << " inputsource_image" << std::endl;
        return 1;
    }

    fs::path inputPath = argv[1];
    auto input = cv::imread(inputPath.string());

    // Checks for valid input
    if (input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return EXIT_FAILURE;
    }
    // TESTING PURPOSE

    DisegniSegmenter segmenter;
    segmenter.setInputImage(input);
    auto fragmentedImage = segmenter.compute();

    // TESTING PURPOSE
    cv::namedWindow("Fragmented Image", 0);
    cv::imshow("Fragmented Image", fragmentedImage[0]);
    cv::waitKey(0);
    return EXIT_SUCCESS;
    // TESTING PURPOSE
}
