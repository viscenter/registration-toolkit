#include "rt/DisegniSegmenter.hpp"

using namespace cv;


int main(int argc, char* argv[]) {


    //TESTING PURPOSE
    cv::Mat input = cv::imread(samples::findFile("simpleShapes.jpg") , cv::IMREAD_COLOR);

    //Checks for valid input
    if( input.empty() )
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return EXIT_FAILURE;
    }
    //TESTING PURPOSE

    DisegniSegmenter a;
    a.setInputImage(input);
    auto fragmentedImage = a.compute();

    //TESTING PURPOSE
    cv::namedWindow("Fragmented Image", 0);
    cv::imshow("Fragmented Image", fragmentedImage[0]);
    cv::waitKey(0);
    return EXIT_SUCCESS;
    //TESTING PURPOSE
}
