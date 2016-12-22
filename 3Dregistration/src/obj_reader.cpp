#include <fstream>
#include <iostream>
#include "obj_reader.hpp"

OBJReader::OBJReader() 
{
    _map = volcart::UVMap(VC_ORIGIN_BOTTOM_LEFT);
}

void OBJReader::Parse(std::string file) 
{
    _map = volcart::UVMap(VC_ORIGIN_BOTTOM_LEFT);
    std::string line;
    std::ifstream i_file (file.c_str());
    if (i_file.is_open())
    {
        while (getline (i_file,line))
        {
            std::vector<std::string> elems;
            Split(line, ' ', elems);
            if (elems[0] == "vt")
            {
                // Parse a texture coordinate
                ParseVT(elems);
            }
            else if (elems[0] == "f")
            {
                // Parse a face
                ParseF(elems);
            }
        }
        i_file.close();
    }
    _map.origin(VC_ORIGIN_TOP_LEFT);
}

void OBJReader::ParseVT(std::vector<std::string> &elems)
{
    // Format:
    assert(elems.size() == 3);
    cv::Vec2d v(stod(elems[1]), stod(elems[2]));
    _uv.push_back(v);
}

void OBJReader::ParseF(std::vector<std::string> &elems)
{
    // Format: f p_id/u_id
    // where p_id = parsed vertex id from f && u_id = parsed uv id from f

    assert(elems.size() == 4);
    for (int i = 1; i < elems.size(); ++i)
    {
        std::vector<std::string> vertex_ref;
        Split(elems[i], '/', vertex_ref);
        // - 1 because obj file starts at 1 and not 0
        if (vertex_ref.size() == 2)
        {
            size_t p_id = std::stoul(vertex_ref[0]) - 1;
            size_t u_id = std::stoul(vertex_ref[1]) - 1;
            _map.set(p_id, _uv[u_id]);
        }
    }
}

void OBJReader::Split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

cv::Vec2d OBJReader::GetUV(size_t p_id)
{
    return _map.get(p_id);
}
