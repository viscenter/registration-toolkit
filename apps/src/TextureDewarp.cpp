#include <vc/texturing/AngleBasedFlattening.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"
#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace fs = rt::filesystem;
namespace vct = volcart::texturing;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " in.obj out.obj step-size"
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Parse command line arguments
    fs::path objPath = argv[1];
    fs::path outPath = argv[2];
    auto sampleRate = std::stof(argv[3]);

    // Load the mesh
    std::cerr << "Reading mesh..." << std::endl;
    rt::io::OBJReader reader;
    reader.setPath(objPath);
    auto mesh = reader.read();
    auto uvMap = reader.getUVMap();
    auto texture = reader.getTextureMat();

    std::cerr << "Flattening mesh..." << std::endl;
    vct::AngleBasedFlattening abf;
    abf.setMesh(mesh);
    abf.compute();
    auto flatMesh = abf.getMesh();

    // Reorder the texture
    std::cerr << "Reordering texture..." << std::endl;
    rt::ReorderUnorganizedTexture r;
    r.setMesh(flatMesh);
    r.setUVMap(uvMap);
    r.setTextureMat(texture);
    r.setSampleRate(sampleRate);
    r.compute();

    // Write to file
    rt::io::OBJWriter writer;
    writer.setPath(outPath);
    writer.setMesh(flatMesh);
    writer.setUVMap(r.getUVMap());
    writer.setTexture(r.getTextureMat());
    writer.write();
}
