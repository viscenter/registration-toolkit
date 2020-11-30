#include "rt/types/UVMap.hpp"

using namespace rt;

/** Top-left UV Origin */
const static cv::Vec2d ORIGIN_TOP_LEFT(0, 0);
/** Top-right UV Origin */
const static cv::Vec2d ORIGIN_TOP_RIGHT(1, 0);
/** Bottom-left UV Origin */
const static cv::Vec2d ORIGIN_BOTTOM_LEFT(0, 1);
/** Bottom-right UV Origin */
const static cv::Vec2d ORIGIN_BOTTOM_RIGHT(1, 1);
/** Get the above vectors when given an Origin enum value */
static cv::Vec2d GetOriginVector(const UVMap::Origin& o);

UVMap::UVMap(UVMap::Origin o) : origin_{o} {}

size_t UVMap::size() const { return uvs_.size(); }

bool UVMap::empty() const { return uvs_.empty(); }

std::vector<cv::Vec2d> UVMap::uvs_as_vector() const { return uvs_; }

std::vector<UVMap::Face> UVMap::faces_as_vector() const { return faces_; }

void UVMap::setOrigin(const UVMap::Origin& o) { origin_ = o; }

UVMap::Origin UVMap::origin() const { return origin_; }

UVMap::Ratio UVMap::ratio() const { return ratio_; }

void UVMap::ratio(double a) { ratio_.aspect = a; }

void UVMap::ratio(double w, double h)
{
    ratio_.width = w;
    ratio_.height = h;
    ratio_.aspect = w / h;
}

size_t UVMap::addUV(const cv::Vec2d& uv, const Origin& o)
{
    // transform to be relative to top-left
    cv::Vec2d transformed;
    cv::absdiff(uv, GetOriginVector(o), transformed);
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
    cv::absdiff(uvs_[id], GetOriginVector(o), transformed);
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

cv::Vec2d GetOriginVector(const UVMap::Origin& o)
{
    switch (o) {
        case UVMap::Origin::TopLeft:
            return ORIGIN_TOP_LEFT;
        case UVMap::Origin::TopRight:
            return ORIGIN_TOP_RIGHT;
        case UVMap::Origin::BottomLeft:
            return ORIGIN_BOTTOM_LEFT;
        case UVMap::Origin::BottomRight:
            return ORIGIN_BOTTOM_RIGHT;
    }
}
