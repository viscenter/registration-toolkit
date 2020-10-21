#pragma once

/** @file */

#include <itkImage.h>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>

namespace rt
{
/**@{*/
/** @brief 8-bit, unsigned, single channel pixel */
using Pixel8UC1 = uint8_t;
/** @brief 8-bit, unsigned, single channel image */
using Image8UC1 = itk::Image<Pixel8UC1, 2>;
/** @brief 8-bit, unsigned, three channel pixel */
using Pixel8UC3 = itk::RGBPixel<uint8_t>;
/** @brief 8-bit, unsigned, three channel image */
using Image8UC3 = itk::Image<Pixel8UC3, 2>;
/** @brief 8-bit, unsigned, four channel pixel */
using Pixel8UC4 = itk::RGBAPixel<uint8_t>;
/** @brief 8-bit, unsigned, four channel image */
using Image8UC4 = itk::Image<Pixel8UC4, 2>;
/**@}*/

/**@{*/
/** @brief 16-bit, unsigned, single channel pixel */
using Pixel16UC1 = uint16_t;
/** @brief 16-bit, unsigned, single channel image */
using Image16UC1 = itk::Image<Pixel16UC1, 2>;
/** @brief 16-bit, unsigned, three channel pixel */
using Pixel16UC3 = itk::RGBPixel<uint16_t>;
/** @brief 16-bit, unsigned, three channel image */
using Image16UC3 = itk::Image<Pixel16UC3, 2>;
/** @brief 16-bit, unsigned, four channel pixel */
using Pixel16UC4 = itk::RGBAPixel<uint16_t>;
/** @brief 16-bit, unsigned, four channel image */
using Image16UC4 = itk::Image<Pixel16UC4, 2>;
/**@}*/

/**@{*/
/** @brief 32-bit, floating point, single channel pixel */
using Pixel32FC1 = float;
/** @brief 32-bit, floating point, single channel image */
using Image32FC1 = itk::Image<Pixel32FC1, 2>;
/** @brief 32-bit, floating point, three channel pixel */
using Pixel32FC3 = itk::RGBPixel<float>;
/** @brief 32-bit, floating point, three channel image */
using Image32FC3 = itk::Image<Pixel32FC3, 2>;
/** @brief 32-bit, floating point, four channel pixel */
using Pixel32FC4 = itk::RGBAPixel<float>;
/** @brief 32-bit, floating point, four channel image */
using Image32FC4 = itk::Image<Pixel32FC4, 2>;
/**@}*/

/**@{*/
/** @brief 64-bit, floating point, 2D vector pixel */
using Vector = itk::Vector<double, 2>;
/** @brief 64-bit, floating point, 2D vector image */
using DeformationField = itk::Image<Vector, 2>;
/**@}*/
}  // namespace rt