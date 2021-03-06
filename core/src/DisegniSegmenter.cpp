#include "rt/DisegniSegmenter.hpp"

#include <algorithm>
#include <exception>
#include <limits>
#include <map>
#include <set>

#include <opencv2/imgproc.hpp>

#include "rt/util/ImageConversion.hpp"

static const int INT_MINI = std::numeric_limits<int>::min();
static const int INT_MAXI = std::numeric_limits<int>::max();
static const cv::Vec3b WHITE = {255, 255, 255};
static const cv::Vec3b BLACK = {0, 0, 0};

using namespace rt;

// Bounding box
struct BoundingBox {
    cv::Point tl{INT_MAXI, INT_MAXI};
    cv::Point br{INT_MINI, INT_MINI};
};

void DisegniSegmenter::setInputImage(const cv::Mat& i) { input_ = i; }

void DisegniSegmenter::setForegroundSeeds(const std::vector<cv::Point>& b)
{
    fgSeeds_ = b;
}

void DisegniSegmenter::setBackgroundSeeds(const std::vector<cv::Point>& b)
{
    bgSeeds_ = b;
}

void DisegniSegmenter::setSeedSize(int s) { seedSize_ = s; }

void DisegniSegmenter::setPreprocessWhiteToBlack(bool b) { whiteToBlack_ = b; }
void DisegniSegmenter::setPreprocessSharpen(bool b) { sharpen_ = b; }
void DisegniSegmenter::setPreprocessBlur(bool b) { blur_ = b; }
void DisegniSegmenter::setBoundingBoxBuffer(int b) { bboxBuffer_ = b; }

auto DisegniSegmenter::compute() -> std::vector<cv::Mat>
{
    auto processed = preprocess_();
    labeled_ = watershed_image_(processed);
    results_ = split_labeled_image_(input_, labeled_);
    return results_;
}

auto DisegniSegmenter::getLabeledImage(bool colored) -> cv::Mat
{
    // Return the raw labels if we don't want a colored image
    if (!colored) {
        return labeled_;
    }

    // Get the unique labels
    std::set<int32_t> uniqueLabels(
        labeled_.begin<int32_t>(), labeled_.end<int32_t>());

    // Generate random colors for each label
    std::map<int32_t, cv::Vec3b> colors;
    for (const auto& l : uniqueLabels) {
        // Border pixels are black
        if (l == -1) {
            colors[l] = cv::Vec3b{0, 0, 0};
        } else {
            auto b = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
            auto g = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
            auto r = static_cast<uint8_t>(cv::theRNG().uniform(0, 256));
            colors[l] = cv::Vec3b{b, g, r};
        }
    }

    // Fill output image with color labels
    cv::Mat dst = cv::Mat::zeros(labeled_.size(), CV_8UC3);
    for (int y = 0; y < labeled_.rows; y++) {
        for (int x = 0; x < labeled_.cols; x++) {
            auto index = labeled_.at<int>(y, x);
            if (colors.count(index) > 0) {
                dst.at<cv::Vec3b>(y, x) = colors.at(index);
            }
        }
    }

    return dst;
}

auto DisegniSegmenter::getOutputImages() const -> std::vector<cv::Mat>
{
    return results_;
}

auto DisegniSegmenter::preprocess_() -> cv::Mat
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
        cv::Mat laplace;
        cv::Mat srcFloat;
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

auto DisegniSegmenter::watershed_image_(const cv::Mat& input) -> cv::Mat
{
    // Setup our label image
    cv::Mat labeled = cv::Mat::zeros(input.size(), CV_32S);

    // Seed our background label with user-provided coords
    for (const auto& coord : bgSeeds_) {
        cv::circle(labeled, coord, seedSize_, cv::Scalar(1), -1);
    }

    // We have two reserved labels and cv::watershed only supports positive
    // integer labels, so protect against too many provided seeds
    if (fgSeeds_.size() > std::numeric_limits<int32_t>::max() - 2) {
        throw std::overflow_error("Number of object seeds exceeds maximum");
    }

    // Seed our foreground labels with user-provided coords
    int32_t label = 2;
    for (const auto& coord : fgSeeds_) {
        cv::circle(labeled, coord, seedSize_, cv::Scalar(label++), -1);
    }

    // Perform the watershed algorithm
    cv::watershed(input, labeled);

    // Return labeled image
    return labeled;
}

auto DisegniSegmenter::split_labeled_image_(
    const cv::Mat& input, const cv::Mat& labeled) -> std::vector<cv::Mat> const
{
    // Setup an alpha channel
    cv::Mat alpha = cv::Mat::zeros(input.size(), CV_32FC1);

    // Find subimage bounding boxes using pixel labels
    std::map<int32_t, BoundingBox> labelBBs;
    for (int y = 0; y < labeled.rows; y++) {
        for (int x = 0; x < labeled.cols; x++) {

            // Get label
            auto label = labeled.at<int32_t>(y, x);

            // Reserved labels:
            // -1: boundary between objects
            //  0: unknown
            //  1: background
            if (label <= 1) {
                continue;
            }

            // Set alpha channel value
            alpha.at<float>(y, x) = 1.F;

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

    // Scale alpha channel to output depth
    alpha = rt::QuantizeImage(alpha, input.depth());

    // Add alpha channel to image
    cv::Mat inputAlpha;
    std::vector<cv::Mat> cns;
    cv::split(input, cns);
    cns.push_back(alpha);
    cv::merge(cns, inputAlpha);

    // Use bounding boxes to create ROI images
    std::vector<cv::Mat> subimgs;
    for (const auto& i : labelBBs) {
        // Apply bbox buffer
        auto minX = std::max(i.second.tl.x - bboxBuffer_, 0);
        auto minY = std::max(i.second.tl.y - bboxBuffer_, 0);
        auto maxX = std::min(i.second.br.x + bboxBuffer_, input.cols - 1);
        auto maxY = std::min(i.second.br.y + bboxBuffer_, input.rows - 1);

        auto height = maxY - minY;
        auto width = maxX - minX;
        cv::Rect roi(minX, minY, width, height);
        cv::Mat subimg = inputAlpha(roi).clone();
        subimgs.push_back(subimg);
    }

    return subimgs;
}