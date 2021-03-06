#pragma once

/** @file */

#include <opencv2/core.hpp>

#include "rt/filesystem.hpp"

namespace rt::io
{

/**
 * @brief Read a TIFF file as uncompressed binary data
 *
 * This function is meant to open TIFF files as a last resort, when
 * other libraries have failed. It was created to handle a set of custom
 * TIFFs that had invalid Compression tags and incorrect StripOffsets.
 * This function assumes that the other important header information
 * (width, height, bit-depth, etc.) is accurate and attempts to read the first
 * TIFF strip as uncompressed binary data.
 *
 *
 * @param path Path to TIFF file
 * @param offset Shift the encoded strip offset by a number of bytes
 */
auto ReadRawTIFF(const filesystem::path& path, int offset = 0) -> cv::Mat;

/**
 * @brief Write a TIFF image to file
 *
 * Supports writing floating point and signed integer TIFFs, in addition to
 * unsigned 8 & 16 bit integer types. Also supports 1-4 channel images. Unless
 * you only need TIFF support, use rt::WriteImage instead.
 */
void WriteTIFF(const filesystem::path& path, const cv::Mat& img);
}  // namespace rt
