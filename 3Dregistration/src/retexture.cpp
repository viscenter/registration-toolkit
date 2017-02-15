//
// Created by Seth Parker on 12/20/16.
//

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vc/core/io/PointSetIO.hpp>
#include <vc/core/io/objWriter.hpp>
#include <vc/core/types/OrderedPointSet.hpp>
#include <vc/core/types/UVMap.hpp>
#include <vc/core/vc_defines.hpp>
#include <vc/meshing/OrderedPointSetMesher.hpp>
#include <vc/texturing/SimpleUV.hpp>

namespace fs = boost::filesystem;
namespace vc = volcart;

int main(int argc, char* argv[]) {

    // Parse args
    fs::path ps_path = argv[1];
    fs::path texture_path = argv[2];

    // Read files
    auto ps = vc::PointSetIO<cv::Vec3d>::ReadOrderedPointSet(ps_path);
    cv::Mat texture_image = cv::imread(texture_path.string(), -1);

    // Generate mesh from PS
    vc::meshing::OrderedPointSetMesher m(ps);
    m.compute();
    auto mesh = m.getOutputMesh();

    // Generate UV Map
    auto uvmap = vc::texturing::SimpleUV(mesh, ps.width(), ps.height());

    // Write the mesh
    fs::path output_path = ps_path.stem();
    output_path += "-";
    output_path += texture_path;
    output_path.replace_extension("obj");
    vc::io::OBJWriter writer;
    writer.setPath(output_path);
    writer.setMesh(mesh);
    writer.setUVMap(uvmap);
    writer.setTexture(texture_image);
    writer.write();
}