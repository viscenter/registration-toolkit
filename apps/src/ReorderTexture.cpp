#include <boost/filesystem.hpp>

#include <vc/core/io/OBJReader.hpp>
#include <vc/core/io/OBJWriter.hpp>
#include <vc/core/types/UVMap.hpp>
#include <vc/core/vc_defines.hpp>
#include <vc/meshing/ITK2VTK.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"

namespace vc = volcart;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " in.obj out.obj" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse command line arguments
    fs::path objPath = argv[1];
    fs::path outPath = argv[2];
    auto sampleRate = std::stof(argv[3]);

    // Load the mesh
    std::cerr << "Reading mesh..." << std::endl;
    vc::io::OBJReader reader;
    reader.setPath(objPath);
    auto mesh = reader.read();
    auto uvMap = reader.getUVMap();
    auto texture = reader.getTextureMat();

    // Reorder the texture
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    vc::meshing::ITK2VTK(mesh, vtkMesh);
    rt::ReorderUnorganizedTexture r;
    r.setMesh(vtkMesh);
    r.setUVMap(uvMap);
    r.setTextureMat(texture);
    r.setSampleRate(sampleRate);
    r.compute();

    // Write to file
    vc::io::OBJWriter writer;
    writer.setPath(outPath);
    writer.setMesh(mesh);
    writer.setUVMap(r.getUVMap());
    writer.setTexture(r.getTextureMat());
    writer.write();
}
