#pragma once

/** @file */

#include <itkCompositeTransform.h>
#include <itkTransform.h>

#include "rt/filesystem.hpp"

namespace rt
{

/** @brief Generic Transform type */
using Transform = itk::Transform<double, 2, 2>;

/** @brief Composite Transform type */
using CompositeTransform = itk::CompositeTransform<double, 2>;

/** @brief Write Transform to a file */
void WriteTransform(
    const filesystem::path& path, const Transform::Pointer& transform);

/** @brief Read Transform from a file */
Transform::Pointer ReadTransform(const filesystem::path& path);
}  // namespace rt
