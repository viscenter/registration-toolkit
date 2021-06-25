#include "rt/io/OBJWriter.hpp"

#include <iostream>

#include "rt/io/ImageIO.hpp"

namespace fs = rt::filesystem;

static constexpr std::size_t UNSET_VALUE = 0;

using namespace rt::io;

OBJWriter::OBJWriter(fs::path outputPath, ITKMesh::Pointer mesh)
    : outputPath_{std::move(outputPath)}, mesh_{std::move(mesh)}
{
}

OBJWriter::OBJWriter(
    fs::path outputPath, ITKMesh::Pointer mesh, rt::UVMap uvMap, cv::Mat uvImg)
    : outputPath_{std::move(outputPath)}
    , mesh_{std::move(mesh)}
    , uvMap_{std::move(uvMap)}
    , texture_{std::move(uvImg)}
{
}

void OBJWriter::setPath(const fs::path& path) { outputPath_ = path; }

void OBJWriter::setUVMap(const rt::UVMap& uvMap) { uvMap_ = uvMap; }

void OBJWriter::setTexture(const cv::Mat& uvImg) { texture_ = uvImg; }

void OBJWriter::setMesh(const ITKMesh::Pointer& mesh) { mesh_ = mesh; }

///// Validation /////
// Make sure that all required parameters have been set and are okay
auto OBJWriter::validate() -> bool
{
    // Make sure the output path has a file extension for the OBJ
    bool hasExt =
        (outputPath_.extension() == ".OBJ" ||
         outputPath_.extension() == ".obj");
    // Make sure the output directory exists
    bool pathExists =
        fs::is_directory(fs::canonical(outputPath_.parent_path()));
    // Check that the mesh exists and has points
    bool meshHasPoints = (mesh_.IsNotNull() && mesh_->GetNumberOfPoints() != 0);

    return (hasExt && pathExists && meshHasPoints);
}

///// Output Methods /////
// Write everything (OBJ, MTL, and PNG) to disk
auto OBJWriter::write() -> int
{
    if (!validate()) {
        return EXIT_FAILURE;
    }

    // Write the OBJ
    write_obj_();

    // Write texture stuff if we have a UV coordinate map
    if (!uvMap_.empty()) {
        write_mtl_();
        write_texture_();
    }

    return EXIT_SUCCESS;
}

// Write the OBJ file to disk
auto OBJWriter::write_obj_() -> int
{
    outputMesh_.open(outputPath_.string());
    if (!outputMesh_.is_open()) {
        return EXIT_FAILURE;
    }

    write_header_();
    write_vertices_();

    // Only write texture information if we have a UV map
    if (!uvMap_.empty()) {
        write_texture_coordinates_();
    }

    write_faces_();
    outputMesh_.close();

    return EXIT_SUCCESS;
}

// Write the MTL file to disk
// See http://paulbourke.net/dataformats/mtl/ for more options
auto OBJWriter::write_mtl_() -> int
{
    fs::path p = outputPath_;
    p.replace_extension("mtl");
    outputMTL_.open(p.string());
    if (!outputMTL_.is_open()) {
        return EXIT_FAILURE;
    }

    // Setup material properties
    // See the following for more info:
    // - http://paulbourke.net/dataformats/mtl/
    // - https://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
    std::cerr << "Writing MTL...\n";
    outputMTL_ << "newmtl default\n";

    // Path to the texture file, relative to the MTL file
    if (!texture_.empty()) {
        outputMTL_ << "\nnewmtl image\n";
        outputMTL_ << "map_Kd " << outputPath_.stem().string() + ".tif\n";
    }

    outputMTL_.close();
    return EXIT_SUCCESS;
}

// Write the PNG texture file to disk
auto OBJWriter::write_texture_() -> int
{
    if (texture_.empty()) {
        return EXIT_FAILURE;
    }

    std::cerr << "Writing texture image...\n";
    fs::path p = outputPath_;
    p.replace_extension("tif");
    rt::WriteImage(p, texture_);
    return EXIT_SUCCESS;
}

// Write our custom header
auto OBJWriter::write_header_() -> int
{
    if (!outputMesh_.is_open()) {
        return EXIT_FAILURE;
    }

    outputMesh_ << "# RT OBJ File\n";
    return EXIT_SUCCESS;
}

// Write the vertex information:
// Vertex: 'v x y z'
// Vertex normal: 'vn nx ny nz'
auto OBJWriter::write_vertices_() -> int
{
    if (!outputMesh_.is_open() || mesh_->GetNumberOfPoints() == 0) {
        return EXIT_FAILURE;
    }
    std::cerr << "Writing vertices...\n";

    outputMesh_ << "# Vertices: " << mesh_->GetNumberOfPoints() << "\n";

    // Iterate over all of the points
    std::size_t vIndex = 1;
    std::size_t vnIndex = 1;
    for (auto pt = mesh_->GetPoints()->Begin(); pt != mesh_->GetPoints()->End();
         pt++) {
        // Make a new point link for this point
        PointLink pointLink(vIndex, UNSET_VALUE, UNSET_VALUE);

        // Write the point position components
        outputMesh_ << "v " << pt.Value()[0] << " " << pt.Value()[1] << " "
                    << pt.Value()[2] << "\n";

        // Write the point normal information
        ITKPixel normal;
        if (mesh_->GetPointData(pt.Index(), &normal)) {
            outputMesh_ << "vn " << normal[0] << " " << normal[1] << " "
                        << normal[2] << "\n";
            pointLink[2] = vnIndex++;
        }

        // Add this vertex to the point links
        pointLinks_.insert({pt.Index(), pointLink});

        ++vIndex;
    }

    return EXIT_SUCCESS;
}

// Write the UV coordinates that will be attached to points: 'vt u v'
auto OBJWriter::write_texture_coordinates_() -> int
{
    if (!outputMesh_.is_open() || uvMap_.empty()) {
        return EXIT_FAILURE;
    }
    std::cerr << "Writing texture coordinates...\n";

    // Ensure coordinates are relative to bottom left
    auto startingOrigin = uvMap_.origin();
    uvMap_.setOrigin(UVMap::Origin::BottomLeft);

    // Write mtl path, relative to OBJ
    auto mtlpath = outputPath_.stem();
    mtlpath.replace_extension("mtl");
    outputMesh_ << "# Texture information\n";
    outputMesh_ << "mtllib " << mtlpath.string() << "\n";

    // Iterate over all of the saved coordinates in our coordinate map
    for (std::size_t pId = 0; pId < uvMap_.size(); ++pId) {
        cv::Vec2d uv = uvMap_.getUV(pId);
        outputMesh_ << "vt " << uv[0] << " " << uv[1] << "\n";
    }

    // Restore the starting origin
    uvMap_.setOrigin(startingOrigin);
    return EXIT_SUCCESS;
}

// Write the face information: 'f v/vt/vn'
auto OBJWriter::write_faces_() -> int
{
    if (!outputMesh_.is_open() || mesh_->GetNumberOfCells() == 0) {
        return EXIT_FAILURE;
    }
    std::cerr << "Writing faces...\n";

    outputMesh_ << "# Faces: " << mesh_->GetNumberOfCells() << "\n";

    bool usingImageMTL = false;
    outputMesh_ << "usemtl default\n";

    // Iterate over the faces of the mesh
    for (auto cell = mesh_->GetCells()->Begin();
         cell != mesh_->GetCells()->End(); ++cell) {
        // Get the UV indices for this face
        auto hasUVFace = not uvMap_.empty() and uvMap_.hasFace(cell.Index());
        UVMap::Face uvFace;
        if (hasUVFace) {
            uvFace = uvMap_.getFace(cell.Index());
        }

        if (not texture_.empty() and hasUVFace and not usingImageMTL) {
            outputMesh_ << "usemtl image\n";
            usingImageMTL = true;
        }

        if (not hasUVFace and usingImageMTL) {
            outputMesh_ << "usemtl default\n";
            usingImageMTL = false;
        }

        // Starts a new face line
        outputMesh_ << "f ";

        // Iterate over the points of this face
        int pIdx{0};
        for (auto* point = cell.Value()->PointIdsBegin();
             point != cell.Value()->PointIdsEnd(); ++point) {

            auto pId = static_cast<std::size_t>(*point);
            auto pointLink = pointLinks_.find(pId)->second;

            outputMesh_ << pointLink[0];

            // Write the vtIndex
            if (hasUVFace) {
                outputMesh_ << "/" << uvFace[pIdx] + 1;
            }

            // Write the vnIndex
            if (pointLink[2] != UNSET_VALUE) {
                // Write a buffer slash if there wasn't a vtIndex
                if (pointLink[1] == UNSET_VALUE) {
                    outputMesh_ << "/";
                }

                outputMesh_ << "/" << pointLink[2];
            }
            pIdx++;
            outputMesh_ << " ";
        }
        outputMesh_ << "\n";
    }

    return EXIT_SUCCESS;
}
