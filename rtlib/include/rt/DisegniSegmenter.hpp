#pragma once

// C++ Standard Libraries
#include <vector>

// External Libraries
#include <opencv2/core.hpp>



/**
 * @brief Isolates individual disegni images from an image containing multiple disegni
 *
 * [This algorithm takes an input Mat image and inserts it into a private class
 * member. This private class member is ran through a watershed algorithm where
 * it's differing color intensities are used to segment the image. After being
 * ran through the watershed algorithm, the image is segmented into different
 * pixel subregions that can be manipulated and displayed through various
 * methods. Next the watersheded image is ran through a split and merge
 * function. The split and merge function took advantage of the differing pixel
 * subregions to insert them into each of their own Mat image. Finally the split
 * Mat images are inserted into a vector and returned as a vector. This vector
 * being the output of the algorithm.]
 */
class DisegniSegmenter
{

public:
    /** @brief Inserts the src image as an input for the algorithm */
    void setInputImage(cv::Mat i);
    /** @brief This calls for segmentation of the src image outputting the fragmented pieces */
    std::vector<cv::Mat> compute();
    /** @brief This retrieves your subregions stored in a vector */
    std::vector<cv::Mat> getOutputImages() const;

private:
    /** This contains the src image to be segmented */
    cv::Mat inputImage_;
    /** This contains the segmented subregions */
    std::vector<cv::Mat> resultImages_;

    /**This conducts the image segmentation using the Watershed Method */
    cv::Mat watershed_image_();
    /**This separates the segmented subregions from the src image into their own images */
    std::vector<cv::Mat> split_(cv::Mat labeledImage);
};
