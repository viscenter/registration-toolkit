#include <gtest/gtest.h>

#include <opencv2/imgcodecs.hpp>
#include "rt/io/ImageIO.hpp"
#include "rt/types/ITKOpenCVBridge.hpp"

using namespace rt;

TEST(ITKOCVBridge, RoundTrip)
{
    auto img = cv::imread("test.png", cv::IMREAD_UNCHANGED);
    auto itkImg = CVMatToITKImage<Image8UC3>(img);
    auto cvImg = ITKImageToCVMat(itkImg);

    WriteImage("output.tif", cvImg);
}