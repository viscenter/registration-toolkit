//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>


namespace rt {
    /**
     * @class Image
     * @brief Represent a simple 2D image for the registration pipeline
     * @ingroup data
     */
    class Image : public Data
    {
    public:
        /** Constructor */
        Image(const boost::filesystem::path& path){ /* Do something to load */}

        /** @brief Return a single image */
        cv::Mat getImage() { return img_; }

        /** @brief Return the number of images we have. Only 1 because Image object represents 1 image */
        int getNumImages() { return 1; }

    private:
        /** Image */
        cv::Mat img_;
    };
}
