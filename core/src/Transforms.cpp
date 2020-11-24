#include "rt/types/Transforms.hpp"

#include <itkCompositeTransformIOHelper.h>
#include <itkTransformFactory.h>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>

using namespace rt;
namespace fs = rt::filesystem;

void rt::WriteTransform(
    const fs::path& path, const Transform::Pointer& transform)
{
    // Always write a composite transform
    auto t = CompositeTransform::New();
    t->AddTransform(transform);
    t->FlattenTransformQueue();

    auto writer = itk::TransformFileWriter::New();
    writer->SetFileName(path.string());
    writer->SetInput(t);
    writer->Update();
}

Transform::Pointer rt::ReadTransform(const fs::path& path)
{
    // Register transforms
    itk::TransformFactoryBase::RegisterDefaultTransforms();

    // Read transform
    auto reader = itk::TransformFileReader::New();
    reader->SetFileName(path.string());
    reader->Update();

    return dynamic_cast<CompositeTransform*>(
        reader->GetTransformList()->begin()->GetPointer());
}