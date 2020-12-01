#include "rt/io/OBJReader.hpp"

#include <regex>
#include <string>

#include <opencv2/imgcodecs.hpp>

#include "rt/types/Exceptions.hpp"
#include "rt/util/String.hpp"

using namespace rt;
using namespace rt::io;

namespace fs = rt::filesystem;

// Constant for validating face values
constexpr static int NOT_PRESENT = -1;
constexpr static size_t VALID_FACE_SIZE = 3;

void OBJReader::setPath(const fs::path& p) { path_ = p; }

ITKMesh::Pointer OBJReader::getMesh() { return mesh_; }

UVMap OBJReader::getUVMap() { return uvMap_; }

// Read the file
ITKMesh::Pointer OBJReader::read()
{
    reset_();
    parse_();
    build_mesh_();
    return mesh_;
}

// Get texture image
cv::Mat OBJReader::getTextureMat()
{
    if (texturePath_.empty() || !fs::exists(texturePath_)) {
        throw IOException("Invalid or unset texture image path");
    }

    return cv::imread(texturePath_.string(), -1);
}

// Prepare all data structures to read a new file
void OBJReader::reset_()
{
    vertices_.clear();
    normals_.clear();
    uvs_.clear();
    faces_.clear();
    texturePath_.clear();
}

// Parse the file
void OBJReader::parse_()
{
    std::regex vertex{"^v"};
    std::regex normal{"^vn"};
    std::regex tcoord{"^vt"};
    std::regex face{"^f"};
    std::regex mtllib("^mtllib");

    std::ifstream ifs(path_.string());
    if (!ifs.good()) {
        throw IOException("Failed to open file for reading");
    }

    std::string line;
    while (std::getline(ifs, line)) {
        // Parse the line
        trim(line);
        auto strs = split(line, ' ');
        std::for_each(
            std::begin(strs), std::end(strs), [](auto& t) { trim(t); });

        // Handle vertices
        if (std::regex_match(strs[0], vertex)) {
            parse_vertex_(strs);
        }

        // Handle normals
        else if (std::regex_match(strs[0], normal)) {
            parse_normal_(strs);
        }

        // Handle texture coordinates
        else if (std::regex_match(strs[0], tcoord)) {
            parse_tcoord_(strs);
        }

        // Handle faces
        else if (std::regex_match(strs[0], face)) {
            parse_face_(strs);
        }

        // Handle mtllib
        else if (std::regex_match(strs[0], mtllib)) {
            parse_mtllib_(strs);
        }
    }

    ifs.close();
}

void OBJReader::parse_vertex_(const std::vector<std::string>& strs)
{
    auto a = std::stod(strs[1]);
    auto b = std::stod(strs[2]);
    auto c = std::stod(strs[3]);
    vertices_.emplace_back(a, b, c);
}

void OBJReader::parse_normal_(const std::vector<std::string>& strs)
{
    auto a = std::stod(strs[1]);
    auto b = std::stod(strs[2]);
    auto c = std::stod(strs[3]);
    normals_.emplace_back(a, b, c);
}

void OBJReader::parse_tcoord_(const std::vector<std::string>& strs)
{
    auto u = std::stod(strs[1]);
    auto v = std::stod(strs[2]);
    uvs_.emplace_back(u, v);
}

void OBJReader::parse_face_(const std::vector<std::string>& strs)
{
    OBJReader::Face f;
    std::vector<std::string> sub(std::begin(strs) + 1, std::end(strs));
    auto faceType = classify_vertref_(sub[0]);

    for (const auto& s : sub) {
        auto vinfo = split(s, '/');
        switch (faceType) {
            case RefType::Vertex:
                f.emplace_back(std::stoi(vinfo[0]), NOT_PRESENT, NOT_PRESENT);
                break;
            case RefType::VertexWithTexture:
                f.emplace_back(
                    std::stoi(vinfo[0]), std::stoi(vinfo[1]), NOT_PRESENT);
                break;
            case RefType::VertexWithNormal:
                f.emplace_back(
                    std::stoi(vinfo[0]), NOT_PRESENT, std::stoi(vinfo[2]));
                break;
            case RefType::VertexWithTextureAndNormal:
                f.emplace_back(
                    std::stoi(vinfo[0]), std::stoi(vinfo[1]),
                    std::stoi(vinfo[2]));
                break;
            case RefType::Invalid:
                throw IOException("Invalid face in obj file");
        }
    }
    faces_.push_back(f);
}

void OBJReader::parse_mtllib_(const std::vector<std::string>& strs)
{
    // Get mtl path, relative to OBJ directory
    // Two canonicals because path_ may be relative as well
    fs::path mtlPath =
        fs::canonical(fs::canonical(path_.parent_path()) / strs[1]);

    // Open the mtl file
    std::ifstream ifs(mtlPath.string());
    if (!ifs.good()) {
        throw IOException("Failed to open mtl file for reading");
    }

    // Find the map_kd line
    std::regex mapKd{"^map_Kd"};

    // Parse the file
    std::string line;
    while (std::getline(ifs, line)) {
        trim(line);
        auto mtlstrs = split(line, ' ');
        std::for_each(
            std::begin(mtlstrs), std::end(mtlstrs), [](auto& t) { trim(t); });

        // Handle map_Kd
        if (std::regex_match(mtlstrs[0], mapKd)) {
            texturePath_ =
                fs::canonical(fs::canonical(path_.parent_path()) / mtlstrs[1]);
        }
        mtlstrs.clear();
    }
    ifs.close();
}

OBJReader::RefType OBJReader::classify_vertref_(const std::string& ref)
{
    const char delimiter = '/';
    auto slashCount = std::count(ref.begin(), ref.end(), delimiter);

    // Invalid slash positions
    if (ref.front() == delimiter || ref.back() == delimiter) {
        throw IOException("Invalid face in obj file");
    }

    // No slashes
    if (slashCount == 0) {
        return RefType::Vertex;
    }

    // One slash
    else if (slashCount == 1) {
        return RefType::VertexWithTexture;
    }

    // Two slashes
    else if (slashCount == 2) {
        // Get the two slash positions
        auto pos0 = ref.find(delimiter, 0);
        auto pos1 = ref.find(delimiter, pos0 + 1);

        // If positions differ by 1, then v//vn
        // else v/vt/vn
        if ((pos1 - pos0) == 1) {
            return RefType::VertexWithNormal;
        } else {
            return RefType::VertexWithTextureAndNormal;
        }
    }
    return RefType::Invalid;
}

void OBJReader::build_mesh_()
{
    // Reset output structures
    uvMap_ = UVMap();
    uvMap_.setOrigin(UVMap::Origin::BottomLeft);
    mesh_ = ITKMesh::New();

    // Add the vertices to the mesh
    if (vertices_.empty()) {
        throw IOException("No vertices in OBJ file");
    }

    ITKMesh::PointIdentifier pid = 0;
    for (auto v : vertices_) {
        mesh_->SetPoint(pid++, v.val);
    }

    for (const auto& uv : uvs_) {
        uvMap_.addUV(uv);
    }

    // Build the faces and UV Map
    // Note: OBJs index vert info from 1
    ITKCell::CellAutoPointer cell;
    ITKMesh::CellIdentifier cid = 0;
    for (auto face : faces_) {
        if (face.size() != VALID_FACE_SIZE) {
            throw IOException("Parsed unsupported, non-triangular face");
        }

        // Setup output objects
        cell.TakeOwnership(new ITKTriangle);
        UVMap::Face uvFace;

        auto idInCell = 0;
        for (auto vinfo : face) {
            if (vinfo[0] - 1 < 0 ||
                vinfo[0] - 1 >= static_cast<int>(vertices_.size())) {
                throw IOException("Out-of-range vertex reference");
            }
            auto vertexID = vinfo[0] - 1;
            cell->SetPointId(idInCell, vertexID);

            if (vinfo[1] != NOT_PRESENT) {
                if (vinfo[1] - 1 < 0 ||
                    vinfo[1] - 1 >= static_cast<int>(uvs_.size())) {
                    throw IOException("Out-of-range UV reference");
                }
                uvFace[idInCell] = vinfo[1] - 1;
            }

            if (vinfo[2] != NOT_PRESENT) {
                if (vinfo[2] - 1 < 0 ||
                    vinfo[2] - 1 >= static_cast<int>(normals_.size())) {
                    throw IOException("Out-of-range normal reference");
                }
                mesh_->SetPointData(vertexID, normals_[vinfo[2] - 1].val);
            }

            idInCell++;
        }
        mesh_->SetCell(cid++, cell);
        uvMap_.addFace(uvFace);
    }
    uvMap_.setOrigin(UVMap::Origin::TopLeft);
}
