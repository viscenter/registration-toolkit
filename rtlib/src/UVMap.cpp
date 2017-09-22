#include "rt/types/UVMap.hpp"

/** Top-left UV Origin */
const static cv::Vec2d ORIGIN_TOP_LEFT(0, 0);
/** Top-right UV Origin */
const static cv::Vec2d ORIGIN_TOP_RIGHT(1, 0);
/** Bottom-left UV Origin */
const static cv::Vec2d ORIGIN_BOTTOM_LEFT(0, 1);
/** Bottom-right UV Origin */
const static cv::Vec2d ORIGIN_BOTTOM_RIGHT(1, 1);

using namespace rt;

size_t UVMap::addUV(const cv::Vec2d& uv, const Origin& o)
{
    // transform to be relative to top-left
    cv::Vec2d transformed;
    cv::absdiff(uv, origin_vector_(o), transformed);
    uvs_.push_back(transformed);

    return uvs_.size() - 1;
}

size_t UVMap::addUV(const cv::Vec2d& uv) { return addUV(uv, origin_); }

cv::Vec2d UVMap::getUV(size_t id, const Origin& o)
{
    if (id >= uvs_.size()) {
        throw std::range_error("uv id not in uv map: " + std::to_string(id));
    }

    // transform to be relative to the provided origin
    cv::Vec2d transformed;
    cv::absdiff(uvs_[id], origin_vector_(o), transformed);
    return transformed;
}

cv::Vec2d UVMap::getUV(size_t id) { return getUV(id, origin_); }

size_t UVMap::addFace(Face f)
{
    faces_.push_back(f);
    return faces_.size() - 1;
}

size_t UVMap::addFace(size_t a, size_t b, size_t c)
{
    faces_.emplace_back(a, b, c);
    return faces_.size() - 1;
}

UVMap::Face UVMap::getFace(size_t id)
{
    if (id >= faces_.size()) {
        throw std::range_error("face id not in uv map");
    }

    return faces_[id];
}

cv::Vec2d UVMap::origin_vector_(const Origin& o)
{
    switch (o) {
        case Origin::TopLeft:
            return ORIGIN_TOP_LEFT;
        case Origin::TopRight:
            return ORIGIN_TOP_RIGHT;
        case Origin::BottomLeft:
            return ORIGIN_BOTTOM_LEFT;
        case Origin::BottomRight:
            return ORIGIN_BOTTOM_RIGHT;
    }
}