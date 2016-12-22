//
// Created by Seth Parker on 12/20/16.
//

#include <boost/filesystem.hpp>
#include <vc/core/vc_defines.h>
#include <vc/core/types/UVMap.h>
#include <vc/core/types/OrderedPointSet.h>
#include <vc/core/io/PointSetIO.h>
#include <vc/core/io/objWriter.h>
#include <vc/meshing/OrderedPointSetMesher.h>
#include <vc/texturing/simpleUV.h>

namespace fs = boost::filesystem;
namespace vc = volcart;

int main(int argc, char* argv[]) {

    // Parse args
    fs::path ps_path = argv[1];
    fs::path texture_path = argv[2];

    // Read files
    auto ps = vc::PointSetIO<vc::Point3d>::ReadOrderedPointSet(ps_path);
    cv::Mat texture_image = cv::imread(texture_path.string(), -1);

    // Generate mesh from PS
    vc::meshing::OrderedPointSetMesher m(ps);
    m.compute();
    auto mesh = m.getOutputMesh();

    // Generate UV Map
    auto uvmap = vc::texturing::simpleUV(mesh, ps.width(), ps.height());

    // Write the mesh
    fs::path output_path = ps_path.stem();
    output_path += "-";
    output_path += texture_path;
    output_path.replace_extension("obj");
    vc::io::objWriter writer;
    writer.setPath(output_path);
    writer.setMesh(mesh);
    writer.setUVMap(uvmap);
    writer.setTexture(texture_image);
    writer.write();
}