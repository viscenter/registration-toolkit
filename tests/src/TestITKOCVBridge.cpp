#include <gtest/gtest.h>

#include <opencv2/imgcodecs.hpp>
#include "rt/io/ImageIO.hpp"
#include "rt/types/ITKOpenCVBridge.hpp"

using namespace rt;

TEST(ITKOCVBridge, RoundTrip)
{
    auto img = cv::imread("gray.tif", cv::IMREAD_UNCHANGED);
    img = rt::QuantizeImage(img, CV_16U);
    WriteImage("quantize.tif", img);
    auto itkImg = CVMatToITKImage<Image16UC3>(img);
    auto cvImg = ITKImageToCVMat(itkImg);

    WriteImage("output.tif", cvImg);
}