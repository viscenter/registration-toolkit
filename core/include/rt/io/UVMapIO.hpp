#pragma once

/** @file */

#include "rt/filesystem.hpp"
#include "rt/types/UVMap.hpp"

namespace rt
{

/** @brief Write a UVMap to a file (.uvm) */
void WriteUVMap(const rt::filesystem::path& path, const UVMap& uvMap);

/** @brief Read a UVMap from a file (.uvm) */
auto ReadUVMap(const rt::filesystem::path& path) -> UVMap;

}  // namespace rt