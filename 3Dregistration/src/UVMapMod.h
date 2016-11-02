// VC UV Map
// Object to store UV mappings for points in a mesh.
// Created by Seth Parker on 10/20/15.

// Internally, all mappings are stored relative to the "top-left" of the 2D
// mapping space. get() & set() transform
// values to and from this relative range based on the value of _origin. By
// setting _origin prior to insertion and
// again prior to retrieval, mappings can be inserted relative to one origin but
// retrieved relative to another.
#pragma once

#include <unordered_map>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace volcart
{

struct Ratio {
    double width, height, aspect;
};

///// UV Maps /////
using Origin = cv::Vec2d;

#define VC_ORIGIN_TOP_LEFT volcart::Origin(0, 0)
#define VC_ORIGIN_TOP_RIGHT volcart::Origin(1, 0)
#define VC_ORIGIN_BOTTOM_LEFT volcart::Origin(0, 1)
#define VC_ORIGIN_BOTTOM_RIGHT volcart::Origin(1, 1)

#define VC_UVMAP_NULL_MAPPING cv::Vec2d(-1, -1)

class UVMap
{
public:
    UVMap() { _origin = VC_ORIGIN_TOP_LEFT; };
    UVMap(Origin o) { _origin = o; };

    size_t size() const { return _map.size(); };
    bool empty() const { return _map.empty(); };

    // Get and set origin
    Origin origin() const { return _origin; };
    void origin(Origin o) { _origin = o; };

    // Set the uv mapping for point p_id
    void set(size_t p_id, cv::Vec2d uv)
    {
        cv::Vec2d transformed;
        // transform to be relative to top-left
        cv::absdiff(uv, _origin, transformed);
        if (_map.find(p_id) == std::end(_map)) {
            _map.insert({p_id, transformed});
        } else {
            _map[p_id] = transformed;
        }
    };

    // Get the uv mapping for point p_id
    cv::Vec2d get(size_t p_id)
    {
        auto it = _map.find(p_id);
        if (it != _map.end()) {
            cv::Vec2d transformed;
            cv::absdiff(
                it->second, _origin,
                transformed);  // transform to be relative to _origin
            return transformed;
        } else
            return VC_UVMAP_NULL_MAPPING;
    };

    // Ratio information
    Ratio ratio() const { return _ratio; };
    void ratio(double a) { _ratio.aspect = a; };
    void ratio(double w, double h)
    {
        _ratio.width = w;
        _ratio.height = h;
        _ratio.aspect = w / h;
    };

    // Plot the UV points on a cv::Mat img
    cv::Mat drawUVMap() const
    {
        int w = std::ceil(_ratio.width);
        int h = std::ceil((double)w / _ratio.aspect);
        cv::Mat r = cv::Mat::zeros(h, w, CV_8UC1);

        for (auto it = std::begin(_map); it != std::end(_map); ++it) {
            cv::Point2d p(it->second[0] * w, it->second[1] * h);
            cv::circle(r, p, 1, 255, -1);
        }

        return r;
    }

private:
    std::unordered_map<size_t, cv::Vec2d> _map;  // holds the mapping
    cv::Vec2d _origin;  // origin inserted and retrieved points are relative to
    Ratio _ratio;
};

}  // volcart
