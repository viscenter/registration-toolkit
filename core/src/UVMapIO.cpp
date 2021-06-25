#include "rt/io/UVMapIO.hpp"

#include <fstream>
#include <regex>
#include <sstream>

#include "rt/types/Exceptions.hpp"
#include "rt/util/String.hpp"

namespace fs = rt::filesystem;

void rt::WriteUVMap(const fs::path& path, const UVMap& uvMap)
{
    std::ofstream ofs{path.string(), std::ios::binary};
    if (!ofs.is_open()) {
        auto msg = "could not open file '" + path.string() + "'";
        throw IOException(msg);
    }

    // Header
    std::stringstream ss;
    ss << "filetype: uvmap" << std::endl;
    ss << "version: 1" << std::endl;
    ss << "type: per-face" << std::endl;
    ss << "size: " << uvMap.size() << std::endl;
    ss << "width: " << uvMap.ratio().width << std::endl;
    ss << "height: " << uvMap.ratio().height << std::endl;
    ss << "origin: " << static_cast<int>(uvMap.origin()) << std::endl;
    ss << "faces: " << uvMap.size_faces() << std::endl;
    ss << "<>" << std::endl;
    ofs << ss.rdbuf();

    // Write the UV coords
    for (const auto& uv : uvMap.uvs_as_vector()) {
        ofs.write(reinterpret_cast<const char*>(uv.val), 2 * sizeof(double));
    }

    // Write the faces
    for (const auto& f : uvMap.faces_as_map()) {
        ofs.write(reinterpret_cast<const char*>(&f.first), sizeof(size_t));
        ofs.write(
            reinterpret_cast<const char*>(f.second.val), 3 * sizeof(size_t));
    }

    ofs.close();
}

auto rt::ReadUVMap(const fs::path& path) -> rt::UVMap
{
    std::ifstream ifs{path.string(), std::ios::binary};
    if (!ifs.is_open()) {
        auto msg = "could not open file '" + path.string() + "'";
        throw IOException(msg);
    }

    struct Header {
        std::string fileType;
        int version;
        std::string type;
        size_t size;
        double width{0};
        double height{0};
        int origin{-1};
        size_t faces;
    };

    // Regexes
    std::regex comments{"^#"};
    std::regex fileType{"^filetype"};
    std::regex version{"^version"};
    std::regex type{"^type"};
    std::regex size{"^size"};
    std::regex width{"^width"};
    std::regex height{"^height"};
    std::regex origin{"^origin"};
    std::regex faces{"^faces"};
    std::regex headerTerminator{"^<>$"};

    Header h;
    std::string line;
    while (std::getline(ifs, line)) {
        trim(line);
        auto strs = split(line, ':');
        std::for_each(
            std::begin(strs), std::end(strs), [](auto& t) { trim(t); });

        // Comments: look like:
        // # This is a comment
        //    # This is another comment
        if (std::regex_match(strs[0], comments)) {
            continue;
        }

        // File type
        else if (std::regex_match(strs[0], fileType)) {
            h.fileType = strs[1];
        }

        // Version
        else if (std::regex_match(strs[0], version)) {
            h.version = std::stoi(strs[1]);
        }

        // Type
        else if (std::regex_match(strs[0], type)) {
            h.type = strs[1];
        }

        // Size
        else if (std::regex_match(strs[0], size)) {
            h.size = std::stoul(strs[1]);
        }

        // Width
        else if (std::regex_match(strs[0], width)) {
            h.width = std::stod(strs[1]);
        }

        // Height
        else if (std::regex_match(strs[0], height)) {
            h.height = std::stod(strs[1]);
        }

        // Origin
        else if (std::regex_match(strs[0], origin)) {
            h.origin = std::stoi(strs[1]);
        }

        // Faces
        else if (std::regex_match(strs[0], faces)) {
            h.faces = std::stoul(strs[1]);
        }

        // End of the header
        else if (std::regex_match(line, headerTerminator)) {
            break;
        }

        // Ignore everything else
        else {
            continue;
        }
    }

    // Sanity check. Do we have a valid UVMap header?
    if (h.fileType.empty()) {
        throw IOException("Must provide file type");
    } else if (h.fileType != "uvmap") {
        throw IOException("File is not a UVMap");
    } else if (h.version != 1) {
        auto msg = "Version mismatch. UVMap file version is " +
                   std::to_string(h.version) + ", processing version is 1.";
        throw IOException(msg);
    } else if (h.type.empty()) {
        throw IOException("Must provide UVMap type");
    } else if (h.type != "per-face") {
        throw IOException("UVMap type not supported: " + h.type);
    } else if (h.width == 0 or h.height == 0) {
        throw IOException("UVMap cannot have dimensions == 0");
    } else if (h.origin == -1) {
        throw IOException("UVMap file does not contain origin");
    }

    // Construct the UVMap
    UVMap map;
    map.setOrigin(static_cast<UVMap::Origin>(h.origin));
    map.ratio(h.width, h.height);

    // Read all of the points
    for (size_t i = 0; i < h.size; i++) {
        std::ignore = i;
        cv::Vec2d uv;
        ifs.read(reinterpret_cast<char*>(uv.val), 2 * sizeof(double));
        map.addUV(uv);
    }

    // Read all of the faces
    for (size_t i = 0; i < h.faces; i++) {
        std::ignore = i;
        std::size_t idx{0};
        ifs.read(reinterpret_cast<char*>(&idx), sizeof(size_t));
        UVMap::Face f;
        ifs.read(reinterpret_cast<char*>(f.val), 3 * sizeof(size_t));
        map.addFace(idx, f);
    }

    return map;
}
