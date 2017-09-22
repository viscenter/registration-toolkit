#include <boost/filesystem.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"
#include "rt/types/ITK2VTK.hpp"
#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace fs = boost::filesystem;

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

    // Reorder the texture
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    rt::ITK2VTK(mesh, vtkMesh);
    rt::ReorderUnorganizedTexture r;
    r.setMesh(vtkMesh);
    r.setUVMap(uvMap);
    r.setTextureMat(texture);
    r.setSampleRate(sampleRate);
    r.compute();

    // Write to file
    rt::io::OBJWriter writer;
    writer.setPath(outPath);
    writer.setMesh(mesh);
    writer.setUVMap(r.getUVMap());
    writer.setTexture(r.getTextureMat());
    writer.write();
}
