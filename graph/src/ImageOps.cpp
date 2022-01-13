#include "rt/graph/ImageOps.hpp"

#include "rt/io/ImageIO.hpp"
#include "rt/util/ImageConversion.hpp"

using namespace rt;

namespace fs = rt::filesystem;
namespace rtg = rt::graph;

rtg::ColorConvertNode::ColorConvertNode()
    : Node{true}, imageIn{&input_}, channels{&cns_}, imageOut{&output_}
{
    registerInputPort("imageIn", imageIn);
    registerInputPort("channels", channels);
    registerOutputPort("imageOut", imageOut);

    compute = [this]() {
        output_ = ColorConvertImage(input_, static_cast<int>(cns_));
    };
}

auto rtg::ColorConvertNode::serialize_(bool useCache, const fs::path& cacheDir)
    -> smgl::Metadata
{
    smgl::Metadata m{{"channels", cns_}};
    if (useCache and not output_.empty()) {
        WriteImage(cacheDir / "converted.tif", output_);
        m["converted"] = "converted.tif";
    }
    return m;
}

void rtg::ColorConvertNode::deserialize_(
    const smgl::Metadata& meta, const fs::path& cacheDir)
{
    cns_ = meta["channels"].get<std::size_t>();
    if (meta.contains("converted")) {
        auto file = meta["converted"].get<std::string>();
        output_ = ReadImage(cacheDir / file);
    }
}