#pragma once

#include <itkImage.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <itkRGBPixel.h>

namespace rt
{
// 8bpc
using Pixel8UC1 = uint8_t;
using Image8UC1 = itk::Image<Pixel8UC1, 2>;
using Pixel8UC3 = itk::RGBPixel<uint8_t>;
using Image8UC3 = itk::Image<Pixel8UC3, 2>;

// 16bpc
using Pixel16UC1 = uint16_t;
using Image16UC1 = itk::Image<Pixel16UC1, 2>;
using Pixel16UC3 = itk::RGBPixel<uint16_t>;
using Image16UC3 = itk::Image<Pixel16UC3, 2>;

// 32bpc
using Pixel32UC1 = float;
using Image32UC1 = itk::Image<Pixel32UC1, 2>;
using Pixel32UC3 = itk::RGBPixel<float>;
using Image32UC3 = itk::Image<Pixel32UC3, 2>;

// Deformation Field
using Vector = itk::Vector<double, 2>;
using DeformationField = itk::Image<Vector, 2>;
}