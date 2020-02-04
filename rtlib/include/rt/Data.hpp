//
// Created by Anthony Tamasi on 2019-10-17.
//

#pragma once

#include <memory>
#include <string>

#include <boost/filesystem.hpp>


namespace et = envitools;
namespace fs = boost::filesystem;


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
        static Pointer Load(const fs::path& path);

        /** @brief Return a single image from the input data */
        virtual cv::Mat getImage(int idx = 0) = 0;

    protected:
        Data() = default;

        /** Constructor */
        Data(const fs::path& path) {}
    };
}
