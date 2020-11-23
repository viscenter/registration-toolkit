#pragma once

#include <itkCompositeTransform.h>
#include <itkTransform.h>

#include "rt/filesystem.hpp"

namespace rt
{

using CompositeTransform = itk::CompositeTransform<double, 2>;
using Transform = itk::Transform<double, 2, 2>;

void WriteTransform(
    const filesystem::path& path, const Transform::Pointer& transform);

CompositeTransform::Pointer ReadTransform(const filesystem::path& path);
}  // namespace rt
