#include "rt/DisegniSegmenter.hpp"

static const int INT_MINI = std::numeric_limits<int>::min();
static const int INT_MAXI = std::numeric_limits<int>::max();
using namespace cv;

// Takes an Input Image and sets it into your private image member
void DisegniSegmenter::setInputImage(Mat i) { inputImage_ = i; }

// Conducts all the private class functions to keep the
// ImageSegmentationMain.cpp concise
std::vector<Mat> DisegniSegmenter::compute()
{
    auto labeledImage = watershed_image_();
    return split_(labeledImage);
}

/*
 * In the study of Image Processing, a watershed is a transformation defined on
a grayscale image. The tool treats the image like a topographic map, with the
brightness of each point representing its height. The tool then finds the lines
that run along the ridges/boundaries of the points of interest. Finally, it
outputs an image with only the points of interest visible.
 */
Mat DisegniSegmenter::watershed_image_()
{
    Mat src = inputImage_;

    //TURN THIS INTO AN OPTION IF YOU HAVE TIME
    // Change the background from white to black, since that will help later to
    // extract better results during the use of Distance Transform
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (src.at<Vec3b>(y, x) == Vec3b(255, 255, 255)) {
                src.at<Vec3b>(y, x)[0] = 0;
                src.at<Vec3b>(y, x)[1] = 0;
                src.at<Vec3b>(y, x)[2] = 0;
            }
        }
    }

    // Create a kernel that we will use to sharpen our image
    Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    // an approximation of second derivative, a quite strong kernel
    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative
    // values BUT a 8bits unsigned int (the one we are working with) can contain
    // values from 0 to 255 so the possible negative number will be truncated
    Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    Mat sharp;
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - imgLaplacian;
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);

    // Median Blur Image
    medianBlur(imgResult, imgResult, 7);

    // Create binary image from source image
    Mat bw;
    cvtColor(imgResult, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);

    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);

    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);
    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);

    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    std::vector<std::vector<Point>> contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++) {
        drawContours(
            markers, contours, static_cast<int>(i),
            Scalar(static_cast<int>(i) + 1), -1);
    }
    // Draw the background marker
    circle(markers, Point(5, 5), 3, Scalar(255), -1);
    markers.convertTo(markers, CV_8U);
    markers.convertTo(markers, CV_32S);

    // Perform the watershed algorithm
    watershed(imgResult, markers);
    Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);

    // Returns the watershedded Mat image as distinct pixel values
    return markers;
}

std::vector<Mat> DisegniSegmenter::split_(Mat watershededImage)
{
    // Finding Boundary Boxes
    struct MinMaxIdentifier {
        cv::Point min{INT_MAXI, INT_MAXI};
        cv::Point max{INT_MINI, INT_MINI};
    };

    // Finding Maximum and Minimum of a subregion
    std::map<int32_t, MinMaxIdentifier> extremeFinder;
    for (int y = 0; y < watershededImage.rows; y++) {
        for (int x = 0; x < watershededImage.cols; x++) {

            // Get label
            auto label = watershededImage.at<int32_t>(y, x);

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
        Rect roi(i.second.min.x, i.second.min.y, width, height);
        Mat image_roi = inputImage_(roi).clone();
        resultImages_.push_back(image_roi);
    }

    // Returns a vector of the fragmented subregions
    return resultImages_;
}
