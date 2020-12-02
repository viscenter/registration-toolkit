#include "rt/io/ImageIO.hpp"

#include <iostream>

#include <opencv2/imgcodecs.hpp>

#include "rt/io/FileExtensionFilter.hpp"
#include "rt/io/TIFFIO.hpp"
#include "rt/util/ImageConversion.hpp"

namespace fs = rt::filesystem;

static const auto IsFormat = rt::FileExtensionFilter;

cv::Mat rt::ReadImage(const fs::path& path)
{
    return cv::imread(path.string(), cv::IMREAD_UNCHANGED);
}

void rt::WriteImage(const fs::path& path, const cv::Mat& img)
{
    // Do nothing on empty images
    if (img.empty()) {
        std::cerr << "Warning: Not writing empty image" << std::endl;
        return;
    }

    // Use our TIFF writer
    if (IsFormat(path, {"tif", "tiff"})) {
        rt::io::WriteTIFF(path, img);
    } else {
        cv::Mat output = img.clone();
        if (img.depth() == CV_32F or img.depth() == CV_64F) {
            std::cerr << "Warning: Image is floating-point but format ";
            std::cerr << path.extension();
            std::cerr << " does not support floating-point images. ";
            std::cerr << "Results may be incorrect.";
            std::cerr << std::endl;
        }

        if (img.channels() == 4 and IsFormat(path, {"jpg", "jpeg"})) {
            std::cerr << "Warning: Image is 4-channel (RGBA) but format ";
            std::cerr << path.extension();
            std::cerr << " does not support 4-channels. ";
            std::cerr << "Extra channel will be removed.";
            std::cerr << std::endl;
            output = rt::ColorConvertImage(img, 3);
        } else if (img.channels() == 2) {
            std::cerr << "Warning: Image is 2-channel (Gray + Alpha) but "
                         "format ";
            std::cerr << path.extension();
            std::cerr << " does not support 2-channels. ";
            std::cerr << "Extra channel will be removed.";
            std::cerr << std::endl;
            output = rt::ColorConvertImage(img, 1);
        }

        cv::imwrite(path.string(), output);
    }
}