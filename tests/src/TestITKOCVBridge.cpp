#include <gtest/gtest.h>

#include <random>

#include <opencv2/core.hpp>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;

class ITKOCVBridge : public testing::TestWithParam<int>
{
};

template <typename ITKPixelValueT, typename CVPixelValueT>
void ComparePixel(const ITKPixelValueT& itk, const CVPixelValueT& cv)
{
    EXPECT_EQ(itk, cv);
}

template <typename ITKPixelValueT, typename CVPixelValueT>
void ComparePixel(
    const itk::RGBPixel<ITKPixelValueT>& itk,
    const cv::Vec<CVPixelValueT, 3>& cv)
{
    for (int d = 0; d < 3; d++) {
        // ITK -> RGB
        // OpenCV -> BGR
        EXPECT_EQ(itk[2 - d], cv[d]);
    }
}

template <typename ITKPixelValueT, typename CVPixelValueT>
void ComparePixel(
    const itk::RGBAPixel<ITKPixelValueT>& itk,
    const cv::Vec<CVPixelValueT, 4>& cv)
{
    for (int d = 0; d < 4; d++) {
        // ITK -> RGB
        // OpenCV -> BGR
        auto itkIdx = (d < 3) ? 2 - d : d;
        EXPECT_EQ(itk[itkIdx], cv[d]);
    }
}

template <typename ITKImageType, typename CVPixelType>
void TestITKConversion(const cv::Mat& m, int size)
{
    auto itkImage = CVMatToITKImage<ITKImageType>(m);
    // Check every pixel
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Get the original value
            auto origVal = m.at<CVPixelType>(y, x);

            // Get the itk value
            typename ITKImageType::IndexType itkXY;
            itkXY[0] = x;
            itkXY[1] = y;
            auto itkVal = itkImage->GetPixel(itkXY);

            ComparePixel(itkVal, origVal);
        }
    }

    cv::Mat cvImage = ITKImageToCVMat(itkImage);
    // Check every pixel
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Get the original opencv value
            auto origVal = m.at<CVPixelType>(y, x);

            // Get the new opencv value
            auto newVal = cvImage.at<CVPixelType>(y, x);

            EXPECT_EQ(newVal, origVal);
        }
    }
}

TEST_P(ITKOCVBridge, RoundTrip)
{
    // Construct matrix
    int dim = 100;
    cv::Mat img = cv::Mat::ones(dim, dim, GetParam());

    // Convert to ITK image
    switch (GetParam()) {
        case CV_8UC1:
            cv::randu(img, 0, 256);
            TestITKConversion<Image8UC1, uint8_t>(img, dim);
            break;
        case CV_8UC3:
            cv::randu(img, cv::Scalar{0, 0, 0}, cv::Scalar{256, 256, 256});
            TestITKConversion<Image8UC3, cv::Vec3b>(img, dim);
            break;
        case CV_8UC4:
            cv::randu(
                img, cv::Scalar{0, 0, 0, 0}, cv::Scalar{256, 256, 256, 256});
            TestITKConversion<Image8UC4, cv::Vec4b>(img, dim);
            break;
        case CV_16UC1:
            cv::randu(img, 0, 65536);
            TestITKConversion<Image16UC1, uint16_t>(img, dim);
            break;
        case CV_16UC3:
            cv::randu(
                img, cv::Scalar{0, 0, 0}, cv::Scalar{65536, 65536, 65536});
            TestITKConversion<Image16UC3, cv::Vec<uint16_t, 3>>(img, dim);
            break;
        case CV_16UC4:
            cv::randu(
                img, cv::Scalar{0, 0, 0, 0},
                cv::Scalar{65536, 65536, 65536, 65536});
            TestITKConversion<Image16UC4, cv::Vec<uint16_t, 4>>(img, dim);
            break;
        case CV_32FC1:
            cv::randu(img, 0, 1.0);
            TestITKConversion<Image32FC1, float>(img, dim);
            break;
        case CV_32FC3:
            cv::randu(img, cv::Scalar{0, 0, 0}, cv::Scalar{1.0, 1.0, 1.0});
            TestITKConversion<Image32FC3, cv::Vec3f>(img, dim);
            break;
        case CV_32FC4:
            cv::randu(
                img, cv::Scalar{0, 0, 0, 0}, cv::Scalar{1.0, 1.0, 1.0, 1.0});
            TestITKConversion<Image32FC4, cv::Vec4f>(img, dim);
            break;
    }
}

INSTANTIATE_TEST_SUITE_P(
    DepthChannelsTest,
    ITKOCVBridge,
    testing::Values(
        CV_8UC1,
        CV_8UC3,
        CV_8UC4,
        CV_16UC1,
        CV_16UC3,
        CV_16UC4,
        CV_32FC1,
        CV_32FC3,
        CV_32FC4));