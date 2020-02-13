//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <boost/filesystem.hpp>

//#include "envi-tools/ENVI.hpp" //IS THIS THE RIGHT WAY TO INCLUDE THIS HEADER?



namespace rt {
    /**
     * @class Data
     * @brief Abstract data structure class to represents the input and
     * output for the registration pipeline
     * @ingroup data
     */
    class Data
    {

    public:
        using Pointer = std::shared_ptr<Data>;

        /** @brief Load the data type from the given file */
        static Pointer Load(const boost::filesystem::path& path);

        /** @brief Return a single image from the input data */
        virtual cv::Mat getImage(int idx = 0) = 0;

        /** @brief Return a single mesh from the input data */
        virtual cv::Mat getMesh(int idx = 0) = 0;

        /** @brief Return a single image from the input data */
        virtual int getNumImages() = 0;

        /** @brief Return a single image from the input data */
        virtual int getNumMeshes() = 0;

    protected:
        Data() = default;

        /** Constructor */
        Data(const boost::filesystem::path& path) {}
    };
}
