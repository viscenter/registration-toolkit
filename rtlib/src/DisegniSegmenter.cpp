#include "rt/DisegniSegmenter.hpp"
#include "itkOtsuMultipleThresholdsCalculator.h"
#include "itkImageFileReader.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileWriter.h"

#include <algorithm>
#include <limits>
#include <map>
#include <set>

#include <opencv2/imgproc.hpp>
#include <ITK-5.0/itkOpenCVImageBridge.h>
#include <rt/ImageTypes.hpp>
#include <ITK-5.0/itkScalarImageToHistogramGenerator.h>
#include <ITK-5.0/itkBinaryThresholdImageFilter.h>

static const int INT_MINI = std::numeric_limits<int>::min();
static const int INT_MAXI = std::numeric_limits<int>::max();
static const cv::Vec3b WHITE = {255, 255, 255};
static const cv::Vec3b BLACK = {0, 0, 0};

using namespace rt;
using OCVB = itk::OpenCVImageBridge;


// Bounding box
struct BoundingBox {
    cv::Point tl{INT_MAXI, INT_MAXI};
    cv::Point br{INT_MINI, INT_MINI};
};

void DisegniSegmenter::setInputImage(const cv::Mat& i) { input_ = i; }

void DisegniSegmenter::setPreprocessWhiteToBlack(bool b) { whiteToBlack_ = b; }
void DisegniSegmenter::setPreprocessSharpen(bool b) { sharpen_ = b; }
void DisegniSegmenter::setPreprocessBlur(bool b) { blur_ = b; }

std::vector<cv::Mat> DisegniSegmenter::compute()
{
    auto processed = preprocess_();
    labeled_ = otsu_segmentation_(processed);
    results_ = split_labeled_image_(input_, labeled_);
    return results_;
}

cv::Mat DisegniSegmenter::getLabeledImage(bool colored)
{
    // Return the raw labels if we don't want a colored image
    if (!colored) {
        return labeled_;
    }

    // Get the unique labels
    std::set<int32_t> unique_labels(
        labeled_.begin<int32_t>(), labeled_.end<int32_t>());

    // Generate random colors for each label
    std::map<int32_t, cv::Vec3b> colors;
    for (const auto& l : unique_labels) {
        auto b = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
        auto g = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
        auto r = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
        colors[l] = cv::Vec3b{b, g, r};
    }

    // Fill output image with color labels
    cv::Mat dst = cv::Mat::zeros(labeled_.size(), CV_8UC3);
    for (int y = 0; y < labeled_.rows; y++) {
        for (int x = 0; x < labeled_.cols; x++) {
            auto index = labeled_.at<int>(y, x);
            if (index > 0 && colors.count(index) > 0) {
                dst.at<cv::Vec3b>(y, x) = colors.at(index);
            }
        }
    }

    return dst;
}

std::vector<cv::Mat> DisegniSegmenter::getOutputImages() const
{
    return results_;
}

cv::Mat DisegniSegmenter::preprocess_()
{
    // Duplicate the input image
    auto processed = input_.clone();

    // Change white pixels to black pixels. Helps images w/white backgrounds
    if (whiteToBlack_) {
        auto it = processed.begin<cv::Vec3b>();
        auto end = processed.end<cv::Vec3b>();
        for (; it != end; ++it) {
            if (*it == WHITE) {
                *it = BLACK;
            }
        }
    }

    // Sharpen using laplacian filter
    if (sharpen_) {
        cv::Mat laplace, srcFloat;
        cv::Mat kernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
        cv::filter2D(processed, laplace, CV_32F, kernel);
        processed.convertTo(srcFloat, CV_32F);
        processed = srcFloat - laplace;
        processed.convertTo(processed, CV_8UC3);
    }

    // Median Blur Image
    if (blur_) {
        cv::medianBlur(processed, processed, 7);
    }

    return processed;
}

cv::Mat DisegniSegmenter::watershed_image_(const cv::Mat& input)
{
    // Create binary image from source image
    cv::Mat bw;
    cv::cvtColor(input, bw, cv::COLOR_BGR2GRAY);
    cv::threshold(bw, bw, 40, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Perform the distance transform algorithm
    cv::Mat dist;
    cv::distanceTransform(bw, dist, cv::DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    cv::normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    cv::threshold(dist, dist, 0.4, 1.0, cv::THRESH_BINARY);
    // Dilate a bit the dist image
    cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8U);
    cv::dilate(dist, dist, kernel1);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(
        dist_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Create the marker image for the watershed algorithm
    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(
            markers, contours, static_cast<int>(i),
            cv::Scalar(static_cast<int>(i) + 1), -1);
    }
    // Draw the background marker
    cv::circle(markers, cv::Point(5, 5), 3, cv::Scalar(255), -1);
    markers.convertTo(markers, CV_32S);

    // Perform the watershed algorithm
    cv::watershed(input, markers);

    // Returns the watershedded Mat image as distinct pixel values
    return markers;
}

std::vector<cv::Mat> DisegniSegmenter::split_labeled_image_(
    const cv::Mat& input, const cv::Mat& labeled)
{
    // Find subimage bounding boxes using pixel labels
    std::map<int32_t, BoundingBox> labelBBs;
    for (int y = 0; y < labeled.rows; y++) {
        for (int x = 0; x < labeled.cols; x++) {

            // Get label
            auto label = labeled.at<int32_t>(y, x);

            // Skip pixels with weird labels
            if (label == -1 || label == 255) {
                continue;
            }

            // Init new bb if we don't have one
            if (labelBBs.count(label) == 0) {
                labelBBs[label] = BoundingBox();
                continue;
            }

            // Update the top-left and bottom-right position for each subimage
            if (x < labelBBs[label].tl.x) {
                labelBBs[label].tl.x = x;
            }
            if (y < labelBBs[label].tl.y) {
                labelBBs[label].tl.y = y;
            }
            if (x > labelBBs[label].br.x) {
                labelBBs[label].br.x = x;
            }
            if (y > labelBBs[label].br.y) {
                labelBBs[label].br.y = y;
            }
        }
    }

    // Use bounding boxes to create ROI images
    std::vector<cv::Mat> subimgs;
    for (const auto& i : labelBBs) {
        auto height = i.second.br.y - i.second.tl.y;
        auto width = i.second.br.x - i.second.tl.x;
        cv::Rect roi(i.second.tl.x, i.second.tl.y, width, height);
        cv::Mat subimg = input(roi).clone();
        subimgs.push_back(subimg);
    }

    return subimgs;
}

cv::Mat DisegniSegmenter::otsu_segmentation_(const cv::Mat& input) {

    int binNumber = 10;
    int thresholdNumber = 150;

    //Conversion of Mat Image into Itk Image (Image8UC3)
    auto inputImage = OCVB::CVMatToITKImage<Image8UC3>(input);

    using FilterType = itk::OtsuMultipleThresholdsImageFilter<Image8UC3, Image8UC1>;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(inputImage);
    filter->SetNumberOfHistogramBins(binNumber);
    filter->SetNumberOfThresholds(thresholdNumber);

    FilterType::ThresholdVectorType thresholds = filter->GetThresholds();

    std::cout << "Thresholds:" << std::endl;

    for (double threshold : thresholds) {
        std::cout << threshold << std::endl;
    }

    std::cout << std::endl;

    return OCVB::ITKImageToCVMat(filter->GetOutput());
}