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

/**
 * @copydoc WriteTransform
 *
 * This version is required on older versions of ITK which don't automatically
 * cast itk::Pointers of derived types to base types.
 */
template <typename T>
void WriteTransform(const filesystem::path& path, const T& transform)
{
    WriteTransform(path, Transform::Pointer(transform.GetPointer()));
}

/** @brief Read Transform from a file */
Transform::Pointer ReadTransform(const filesystem::path& path);
}  // namespace rt
