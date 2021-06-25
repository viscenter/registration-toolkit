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
static auto GetOriginVector(const UVMap::Origin& o) -> cv::Vec2d;

UVMap::UVMap(UVMap::Origin o) : origin_{o} {}

auto UVMap::size() const -> size_t { return uvs_.size(); }

auto UVMap::size_faces() const -> size_t { return faces_.size(); }

auto UVMap::empty() const -> bool { return uvs_.empty(); }

auto UVMap::uvs_as_vector() const -> std::vector<cv::Vec2d> { return uvs_; }

auto UVMap::faces_as_map() const -> std::unordered_map<std::size_t, UVMap::Face>
{
    return faces_;
}

void UVMap::setOrigin(const UVMap::Origin& o) { origin_ = o; }

auto UVMap::origin() const -> UVMap::Origin { return origin_; }

auto UVMap::ratio() const -> UVMap::Ratio { return ratio_; }

void UVMap::ratio(double a) { ratio_.aspect = a; }

void UVMap::ratio(double w, double h)
{
    ratio_.width = w;
    ratio_.height = h;
    ratio_.aspect = w / h;
}

auto UVMap::addUV(const cv::Vec2d& uv, const Origin& o) -> size_t
{
    // transform to be relative to top-left
    cv::Vec2d transformed;
    cv::absdiff(uv, GetOriginVector(o), transformed);
    uvs_.push_back(transformed);

    return uvs_.size() - 1;
}

auto UVMap::addUV(const cv::Vec2d& uv) -> size_t { return addUV(uv, origin_); }

auto UVMap::getUV(size_t id, const Origin& o) const -> cv::Vec2d
{
    if (id >= uvs_.size()) {
        throw std::range_error("uv id not in uv map: " + std::to_string(id));
    }

    // transform to be relative to the provided origin
    cv::Vec2d transformed;
    cv::absdiff(uvs_[id], GetOriginVector(o), transformed);
    return transformed;
}

auto UVMap::getUV(size_t id) const -> cv::Vec2d { return getUV(id, origin_); }

auto UVMap::addFace(std::size_t idx, const Face& f) -> size_t
{
    faces_[idx] = f;
    return idx;
}

auto UVMap::addFace(size_t a, size_t b, size_t c) -> size_t
{
    auto idx = faces_.size();
    while (faces_.count(idx) > 0) {
        idx++;
    }
    faces_[idx] = {a, b, c};
    return idx;
}

auto UVMap::hasFace(std::size_t idx) const -> bool
{
    return faces_.count(idx) > 0;
}

auto UVMap::getFace(size_t id) const -> UVMap::Face { return faces_.at(id); }

auto UVMap::getFaceUVs(std::size_t id) const -> std::vector<cv::Vec2d>
{
    auto f = faces_.at(id);
    return {uvs_.at(f[0]), uvs_.at(f[1]), uvs_.at(f[2])};
}

auto GetOriginVector(const UVMap::Origin& o) -> cv::Vec2d
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
