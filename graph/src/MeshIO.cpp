#include "rt/graph/MeshIO.hpp"

#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"

using namespace rt;

namespace fs = rt::filesystem;
namespace rtg = rt::graph;

rtg::MeshReadNode::MeshReadNode()
{
    registerInputPort("path", path);
    registerOutputPort("mesh", mesh);
    registerOutputPort("image", image);
    registerOutputPort("uvMap", uvMap);
    compute = [this]() {
        std::cout << "Reading mesh..." << std::endl;
        io::OBJReader r;
        r.setPath(path_);
        mesh_ = r.read();
        img_ = r.getTextureMat();
        uv_ = r.getUVMap();
    };
}

smgl::Metadata rtg::MeshReadNode::serialize_(bool, const fs::path&)
{
    return {{"path", path_.string()}};
}

void rtg::MeshReadNode::deserialize_(
    const smgl::Metadata& meta, const fs::path&)
{
    path_ = meta["path"].get<std::string>();
    compute();
}

rtg::MeshWriteNode::MeshWriteNode()
{
    registerInputPort("path", path);
    registerInputPort("mesh", mesh);
    registerInputPort("image", image);
    registerInputPort("uvMap", uvMap);
    compute = [this]() {
        std::cout << "Writing mesh..." << std::endl;
        io::OBJWriter w;
        w.setPath(path_);
        w.setMesh(mesh_);
        w.setTexture(img_);
        w.setUVMap(uv_);
        w.write();
    };
}

smgl::Metadata rtg::MeshWriteNode::serialize_(bool, const fs::path&)
{
    return {{"path", path_.string()}};
}

void rtg::MeshWriteNode::deserialize_(
    const smgl::Metadata& meta, const fs::path&)
{
    path_ = meta["path"].get<std::string>();
}