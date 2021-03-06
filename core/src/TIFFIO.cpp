#include "rt/io/TIFFIO.hpp"

#include <cstring>

#include <opencv2/imgproc.hpp>

#include "rt/Version.hpp"
#include "rt/util/String.hpp"

// Wrapping in a namespace to avoid define collisions
namespace lt
{
#include <tiffio.h>
}

using namespace rt;
namespace fs = rt::filesystem;

// Return a CV Mat type using TIF type (signed, unsigned, float),
// bit-depth, and number of channels
static auto GetCVMatType(
    const uint16_t tifType, const uint16_t depth, const uint16_t channels)
    -> int
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
auto io::ReadRawTIFF(const fs::path& path, int offset) -> cv::Mat
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
        uint32_t* offsets{nullptr};
        TIFFGetField(tif, TIFFTAG_STRIPOFFSETS, &offsets);
        offsets[0] += offset;
        TIFFSetField(tif, TIFFTAG_STRIPOFFSETS, offsets);
    }

    // Read only the first strip
    uint32_t* bc{nullptr};
    lt::tstrip_t strip = 0;
    TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
    uint32_t stripsize = bc[strip];
    lt::tdata_t buf = lt::_TIFFmalloc(stripsize);
    TIFFReadRawStrip(tif, strip, buf, bc[strip]);

    // Put into the cv::Mat
    auto h = static_cast<int>(height);
    auto w = static_cast<int>(width);
    output = cv::Mat(h, w, cvType, buf);

    lt::_TIFFfree(buf);
    lt::TIFFClose(tif);

    return output;
}

// Write a TIFF to a file. This implementation heavily borrows from how OpenCV's
// TIFFEncoder writes to the TIFF
void io::WriteTIFF(const fs::path& path, const cv::Mat& img)
{
    // Safety checks
    if (img.channels() < 1 or img.channels() > 4) {
        throw std::runtime_error("Unsupported number of channels");
    }

    auto ext = path.extension().string();
    to_upper(ext);
    if (ext != ".TIF" && ext != ".TIFF") {
        throw std::runtime_error("Invalid file extension " + ext);
    }

    // Image metadata
    auto channels = img.channels();
    auto width = static_cast<unsigned>(img.cols);
    auto height = static_cast<unsigned>(img.rows);
    auto rowsPerStrip = height;

    // Sample format
    int bitsPerSample{-1};
    int sampleFormat{-1};
    switch (img.depth()) {
        case CV_8U:
            sampleFormat = SAMPLEFORMAT_UINT;
            bitsPerSample = 8;
            break;
        case CV_8S:
            sampleFormat = SAMPLEFORMAT_INT;
            bitsPerSample = 8;
            break;
        case CV_16U:
            sampleFormat = SAMPLEFORMAT_UINT;
            bitsPerSample = 16;
            break;
        case CV_16S:
            sampleFormat = SAMPLEFORMAT_INT;
            bitsPerSample = 16;
            break;
        case CV_32S:
            sampleFormat = SAMPLEFORMAT_INT;
            bitsPerSample = 32;
            break;
        case CV_32F:
            sampleFormat = SAMPLEFORMAT_IEEEFP;
            bitsPerSample = 32;
            break;
        case CV_64F:
            sampleFormat = SAMPLEFORMAT_IEEEFP;
            bitsPerSample = 64;
            break;
        default:
            throw std::runtime_error("Unsupported image depth");
    }

    // Photometric Interpretation
    int photometric{-1};
    switch (channels) {
        case 1:
        case 2:
            photometric = PHOTOMETRIC_MINISBLACK;
            break;
        case 3:
        case 4:
            photometric = PHOTOMETRIC_RGB;
            break;
        default:
            throw std::runtime_error("Unsupported number of channels");
    }

    // Open the file
    auto* out = lt::TIFFOpen(path.c_str(), "w");
    if (out == nullptr) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // Encoding parameters
    lt::TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    lt::TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    lt::TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);
    lt::TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    lt::TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    lt::TIFFSetField(out, TIFFTAG_SAMPLEFORMAT, sampleFormat);
    lt::TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
    lt::TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, channels);
    lt::TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);

    // Add alpha tag data
    // TODO: Let user decide associated/unassociated tag
    // See TIFF 6.0 spec, section 18
    if (channels == 2 or channels == 4) {
        std::array<uint16_t, 1> tag{EXTRASAMPLE_UNASSALPHA};
        lt::TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 1, tag.data());
    }

    // Metadata
    lt::TIFFSetField(
        out, TIFFTAG_SOFTWARE, ProjectInfo::NameAndVersion().c_str());

    // Row buffer. OpenCV documentation mentions that TIFFWriteScanline
    // modifies its read buffer, so we can't use the cv::Mat directly
    auto bufferSize = static_cast<size_t>(lt::TIFFScanlineSize(out));
    std::vector<char> buffer(bufferSize + 32);

    // Get working copy with converted channels if an RGB-type image
    cv::Mat imgCopy;
    if (img.channels() == 3) {
        cv::cvtColor(img, imgCopy, cv::COLOR_BGR2RGB);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, imgCopy, cv::COLOR_BGRA2RGBA);
    } else {
        imgCopy = img;
    }

    // For each row
    for (unsigned row = 0; row < height; row++) {
        std::memcpy(&buffer[0], imgCopy.ptr(static_cast<int>(row)), bufferSize);
        auto result = lt::TIFFWriteScanline(out, &buffer[0], row, 0);
        if (result == -1) {
            lt::TIFFClose(out);
            auto msg = "Failed to write row " + std::to_string(row);
            throw std::runtime_error(msg);
        }
    }

    // Close the tiff
    lt::TIFFClose(out);
}