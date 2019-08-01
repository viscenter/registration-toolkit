#include "rt/io/ReadRawTIFF.hpp"

#include <iostream>

// Wrapping in a namespace to avoid define collisions
namespace lt
{
#include <tiffio.h>
}

using namespace rt;
namespace fs = boost::filesystem;

// Return a CV Mat type using TIF type (signed, unsigned, float),
// bit-depth, and number of channels
static int GetCVMatType(
    const uint16_t tifType, const uint16_t depth, const uint16_t channels)
{
    switch (depth) {
        case 8:
            if (tifType == SAMPLEFORMAT_INT) {
                return CV_MAKETYPE(CV_8S, channels);
            } else {
                return CV_MAKETYPE(CV_8U, channels);
            }
        case 16:
            if (tifType == SAMPLEFORMAT_INT) {
                return CV_MAKETYPE(CV_16S, channels);
            } else {
                return CV_MAKETYPE(CV_16U, channels);
            }
        case 32:
            if (tifType == SAMPLEFORMAT_INT) {
                return CV_MAKETYPE(CV_32S, channels);
            } else {
                return CV_MAKETYPE(CV_32F, channels);
            }
        default:
            return CV_8UC3;
    }
}

// Read a TIFF strip
cv::Mat io::ReadRawTIFF(const fs::path& path, int offset)
{
    // Make sure input file exists
    if (!fs::exists(path)) {
        throw std::runtime_error("File does not exist");
    }

    // Setup output
    cv::Mat output;

    // Open the file read-only
    lt::TIFF* tif = lt::TIFFOpen(path.c_str(), "r");
    if (tif == nullptr) {
        throw std::runtime_error("Failed to open tif");
    }

    // Get metadata
    uint32_t width = 0;
    uint32_t height = 0;
    uint16_t type = 1;
    uint16_t depth = 1;
    uint16_t channels = 1;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &type);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &depth);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &channels);
    auto cvType = GetCVMatType(type, depth, channels);

    // Apply offset to first strip
    if (offset != 0) {
        uint32_t* offsets;
        TIFFGetField(tif, TIFFTAG_STRIPOFFSETS, &offsets);
        offsets[0] += offset;
        TIFFSetField(tif, TIFFTAG_STRIPOFFSETS, offsets);
    }

    // Read only the first strip
    uint32_t* bc;
    lt::tstrip_t strip = 0;
    TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
    uint32_t stripsize = bc[strip];
    lt::tdata_t buf = lt::_TIFFmalloc(stripsize);
    TIFFReadRawStrip(tif, strip, buf, bc[strip]);

    // Put into the cv::Mat
    output = cv::Mat(height, width, cvType, buf);

    lt::_TIFFfree(buf);
    lt::TIFFClose(tif);

    return output;
}
