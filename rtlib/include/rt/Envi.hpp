//
// Created by Anthony Tamasi on 2020-02-04.
//

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include <envi-tools/ENVI.hpp>


namespace rt {
    /**
     * @class EVNI
     * @brief Represent an ENVI hyperspectral image for the registration pipeline
     * @ingroup data
     */
    class ENVI : public Data
    {
    public:
        /** Constructor */
        ENVI(const fs::path& path){}
        //cv::Mat getImage(int idx = 0) { return bands_[idx]; } //Could use this if we read bands into vector. But would also need functions to load into vector, and access vector.

        /** @brief Return a single image from the input data */
        cv::Mat getImage(int idx = 0) {}

        /** @brief Return the number of images we have available */
        int getNumImages() { return envi_.getWavelengths.size(); }

    private:
        //std::vector<cv::Mat> bands_; //Could use to store bands we have read in memory?
        /** ENVI object */
        et::ENVI envi_;
    };
}
