#pragma once

#include <boost/filesystem.hpp>
#include <itkCompositeTransform.h>
#include <itkTransform.h>

namespace rt
{

using CompositeTransform = itk::CompositeTransform<double, 2>;
using Transform = itk::Transform<double, 2, 2>;

void WriteTransform(
    const boost::filesystem::path& path, const Transform::Pointer& transform);

CompositeTransform::Pointer ReadTransform(const boost::filesystem::path& path);
}  // namespace rt
