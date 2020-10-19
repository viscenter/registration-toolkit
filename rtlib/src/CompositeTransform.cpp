#include "rt/types/CompositeTransform.hpp"

#include <itkCompositeTransformIOHelper.h>
#include <itkTransformFactory.h>
#include <itkTransformFileReader.h>
#include <itkTransformFileWriter.h>

using namespace rt;
namespace rtg = rt::graph;
namespace fs = boost::filesystem;

void rt::WriteTransform(
    const fs::path& path,
    const CompositeTransform::TransformTypePointer& transform)
{
    auto writer = itk::TransformFileWriter::New();
    writer->SetFileName(path.string());
    writer->SetInput(transform);
    writer->Update();
}

CompositeTransform::Pointer rt::ReadTransform(const fs::path& path)
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

rtg::CompositeTransformNode::CompositeTransformNode()
{
    registerInputPort("lhs", lhs);
    registerInputPort("rhs", rhs);
    registerOutputPort("result", result);

    compute = [=]() {
        combined_ = CompositeTransform::New();
        if (first_) {
            combined_->AddTransform(first_);
        }
        if (second_) {
            combined_->AddTransform(second_);
        }
    };
}

smgl::Metadata rtg::CompositeTransformNode::serialize_(
    bool useCache, const Path& cacheDir)
{
    Metadata m;
    if (useCache) {
        WriteTransform(cacheDir / "composite.tfm", combined_);
        m["transform"] = "composite.tfm";
    }
    return m;
}

void rtg::CompositeTransformNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        combined_ = ReadTransform(cacheDir / file);
    }
}

rtg::TransformLandmarksNode::TransformLandmarksNode()
{
    registerInputPort("transform", transform);
    registerInputPort("landmarksIn", landmarksIn);
    registerOutputPort("landmarksOut", landmarksOut);

    compute = [this]() {
        ldmOut_.clear();
        auto i = tfm_->GetInverseTransform();
        for (const auto& p : ldmIn_) {
            ldmOut_.emplace_back(i->TransformPoint(p));
        }
    };
}

smgl::Metadata rtg::TransformLandmarksNode::serialize_(
    bool useCache, const Path& cacheDir)
{
    // TODO: Implement
    return Metadata::object();
}

void rtg::TransformLandmarksNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
{
    // TODO:: Implement
}