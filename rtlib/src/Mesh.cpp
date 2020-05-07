//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Mesh.hpp"

#include "rt/io/OBJReader.hpp"
#include "rt/types/ITKMesh.hpp"

using namespace rt;
namespace fs = boost::filesystem;

Mesh::Mesh(const fs::path& path)
{
    // Read in mesh along with its texture
    io::OBJReader reader;
    reader.setPath(path);

    try {
        mesh_ = reader.read();
        texture_ = reader.getTextureMat();
        uvMap_ = reader.getUVMap();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
