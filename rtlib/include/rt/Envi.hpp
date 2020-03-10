//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "rt/Data.hpp"


namespace rt {
    /**
     * @class ENVI
     * @brief Represent an ENVI hyperspectral image for the registration pipeline
     * @ingroup data
     */
class ENVI : public Data
    {
    public:
        /** Constructor */
        ENVI(const boost::filesystem::path& path){}
        //cv::Mat getImage(int idx = 0) { return bands_[idx]; } //Could use this if we read bands into vector. But would also need functions to load into vector, and access vector.

        /** @brief Return a single image from the input data */
        cv::Mat getImage(int idx = 0) {}

        /** @brief Return the number of images we have available */
        int getNumImages() { return envi_.getWavelengths.size(); }

        /** @brief Return whether or not this object has a mesh */
        bool hasMesh() { return false; }

    private:
        //std::vector<cv::Mat> bands_; //Could use to store bands we have read in memory?
        /** ENVI object */
        envitools::ENVI envi_;
    };
}
