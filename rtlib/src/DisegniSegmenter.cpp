#include "rt/DisegniSegmenter.hpp"

#include <limits>
#include <map>
#include <iostream>

#include <opencv2/imgproc.hpp>

static const int INT_MINI = std::numeric_limits<int>::min();
static const int INT_MAXI = std::numeric_limits<int>::max();

// Finding Boundary Boxes
struct MinMaxIdentifier {
    cv::Point min{INT_MAXI, INT_MAXI};
    cv::Point max{INT_MINI, INT_MINI};
};

// Takes an Input Image and sets it into your private image member
void DisegniSegmenter::setInputImage(cv::Mat i) { inputImage_ = i; }

//This returns the subregions you have segemented in a vector container
std::vector<cv::Mat> DisegniSegmenter::getOutputImages() const { return resultImages_; }

// Conducts all the private class functions to keep the
// ImageSegmentationMain.cpp concise
std::vector<cv::Mat> DisegniSegmenter::compute()
{
    auto labeledImage = watershed_image_();
    return split_(labeledImage);
}

/*
 * In the study of Image Processing, a watershed is a transformation defined on
a gray-scale image. The tool treats the image like a topographic map, with the
brightness of each point representing its height. The tool then finds the lines
that run along the ridges/boundaries of the points of interest. Finally, it
outputs an image with only the points of interest visible.

 This method is heavily based off of the OpenCv tutorial "Image Segmentation with Distance Transform
 and Watershed Algorithm" available at : https://docs.opencv.org/3.4/d2/dbd/tutorial_distance_transform.html
 */
cv::Mat DisegniSegmenter::watershed_image_() {
    cv::Mat src = inputImage_;

    // Change the background from white to black, since that will help later to
    // extract better results during the use of Distance Transform
    int menuLoop = 1;
    char userInput;
    do {
        std::cout << "Would you like to change invert the background of your src image? (Y or N):";
        std::cin >> userInput;

        if (userInput == 'Y' || userInput == 'y'){
            for (int y = 0; y < src.rows; y++) {
                for (int x = 0; x < src.cols; x++) {
                    if (src.at<cv::Vec3b>(y, x) == cv::Vec3b(255, 255, 255)) {
                        src.at<cv::Vec3b>(y, x)[0] = 0;
                        src.at<cv::Vec3b>(y, x)[1] = 0;
                        src.at<cv::Vec3b>(y, x)[2] = 0;
                    }
                }
            }
            menuLoop = 0;
        } else if (userInput == 'N' || userInput == 'n') {
            menuLoop = 0;
        } else {
            std::cout << "Enter a valid input." << std::endl;
        }
    } while (menuLoop == 1);

    // Create a kernel that we will use to sharpen our image
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    // an approximation of second derivative, a quite strong kernel
    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative
    // values BUT a 8bits unsigned int (the one we are working with) can contain
    // values from 0 to 255 so the possible negative number will be truncated
    cv::Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    cv::Mat sharp;
    src.convertTo(sharp, CV_32F);
    cv::Mat imgResult = sharp - imgLaplacian;
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);

    // Median Blur Image
    medianBlur(imgResult, imgResult, 7);

    // Create binary image from source image
    cv::Mat bw;
    cvtColor(imgResult, bw, cv::COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Perform the distance transform algorithm
    cv::Mat dist;
    distanceTransform(bw, dist, cv::DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1.0, cv::NORM_MINMAX);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, 0.4, 1.0, cv::THRESH_BINARY);
    // Dilate a bit the dist image
    cv::Mat kernel1 = cv::Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    cv::Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    std::vector<std::vector<cv::Point>> contours;
    findContours(dist_8u, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    cv::Mat markers = cv::Mat::zeros(dist.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++) {
        drawContours(
            markers, contours, static_cast<int>(i),
            cv::Scalar(static_cast<int>(i) + 1), -1);
    }
    // Draw the background marker
    circle(markers, cv::Point(5, 5), 3, cv::Scalar(255), -1);
    markers.convertTo(markers, CV_8U);
    markers.convertTo(markers, CV_32S);

    // Perform the watershed algorithm
    watershed(imgResult, markers);
    cv::Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);

    // Returns the watershedded Mat image as distinct pixel values
    return markers;
}

std::vector<cv::Mat> DisegniSegmenter::split_(cv::Mat labeledImage)
{
    // Finding Maximum and Minimum of a subregion
    std::map<int32_t, MinMaxIdentifier> extremeFinder;
    for (int y = 0; y < labeledImage.rows; y++) {
        for (int x = 0; x < labeledImage.cols; x++) {

            // Get label
            auto label = labeledImage.at<int32_t>(y, x);

            // Labels that are skipped because they aren't a part of the
            // subregion
            if (label == -1 || label == 255) {
                continue;
            }

            // Make a new minmax if we don't have one for this label
            if (extremeFinder.count(label) == 0) {
                extremeFinder[label] = MinMaxIdentifier();
                continue;
            }

            // This finds the max (top-left point) and min (bottom-right point)
            // for each subregion
            if (x < extremeFinder[label].min.x) {
                extremeFinder[label].min.x = x;
            };
            if (y < extremeFinder[label].min.y) {
                extremeFinder[label].min.y = y;
            };
            if (x > extremeFinder[label].max.x) {
                extremeFinder[label].max.x = x;
            };
            if (y > extremeFinder[label].max.y) {
                extremeFinder[label].max.y = y;
            };
        }
    }

    // Region of Interest taking Minimum and Maximum points
    for (const auto& i : extremeFinder) {
        int height = ((i.second.max.y) - (i.second.min.y));
        int width = ((i.second.max.x) - (i.second.min.x));
        cv::Rect roi(i.second.min.x, i.second.min.y, width, height);
        cv::Mat image_roi = inputImage_(roi).clone();
        resultImages_.push_back(image_roi);
    }

    // Returns a vector of the fragmented subregions
    return resultImages_;
}
