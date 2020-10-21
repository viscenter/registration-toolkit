#include "rt/io/LandmarkWriter.hpp"

#include <iostream>

using namespace rt;
namespace rtg = rt::graph;
namespace fs = boost::filesystem;

void LandmarkWriter::setPath(const fs::path& p) { path_ = p; }

void LandmarkWriter::setFixedLandmarks(const LandmarkContainer& f)
{
    fixed_ = f;
}

void LandmarkWriter::setMovingLandmarks(const LandmarkContainer& m)

{
    moving_ = m;
}

void LandmarkWriter::write()
{
    // Check parameters
    if (path_.empty()) {
        throw std::runtime_error("Empty file path");
    }

    if (fixed_.size() != moving_.size()) {
        throw std::runtime_error("Landmark containers of different size");
    }

    // Open file
    std::ofstream file(path_.string());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open output file");
    }

    // Write the landmarks
    for (size_t i = 0; i < fixed_.size(); i++) {
        file << fixed_.at(i)[0] << " ";
        file << fixed_.at(i)[1] << " ";
        file << moving_.at(i)[0] << " ";
        file << moving_.at(i)[1] << "\n";
    }

    // Close file
    file.close();
}

rtg::LandmarkWriterNode::LandmarkWriterNode()
{
    registerInputPort("path", path);
    registerInputPort("fixed", fixed);
    registerInputPort("moving", moving);
    compute = [this]() {
        std::cout << "Writing landmarks to file..." << std::endl;
        writer_.setPath(path_);
        writer_.setFixedLandmarks(fixed_);
        writer_.setMovingLandmarks(moving_);
        writer_.write();
    };
}

smgl::Metadata rtg::LandmarkWriterNode::serialize_(bool, const Path&)
{
    return {{"path", path_.string()}};
}

void rtg::LandmarkWriterNode::deserialize_(const Metadata& meta, const Path&)
{
    path_ = meta["path"].get<std::string>();
}
