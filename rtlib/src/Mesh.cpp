//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Mesh.hpp"

using namespace rt;


Data::Mesh(const fs::path& path)
{
    //Read in mesh along with its texture
    io::OBJReader reader;
    reader.setPath(path);

    try {
        origMesh_ = reader.read();
        cvFixedImage_ = reader.getTextureMat();
        fixedImage_ = OCVBridge::CVMatToITKImage<Image8UC3>(cvFixedImage_);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}


