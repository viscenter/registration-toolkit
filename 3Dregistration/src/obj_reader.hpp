#ifndef OBJ_READER_H_
#define OBJ_READER_H_

#include <vector>
#include <string>
#include "UVMapMod.h"

class OBJReader
{
public:
	OBJReader();
	void Parse(std::string file);
	cv::Vec2d GetUV(size_t p_id);

private:
	void Split(const std::string &s, char delim, std::vector<std::string> &elems);
	void ParseVT(std::vector<std::string> &elems);
	void ParseF(std::vector<std::string> &elems);

	volcart::UVMap _map;
	std::vector<cv::Vec2d> _uv;
};

#endif //OBJ_READER_H_