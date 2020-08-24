#pragma once

#include <string>

namespace rt
{

struct ProjectInfo {
    static std::string Name();
    static std::string VersionString();
    static std::string NameAndVersion();
    static uint32_t VersionMajor();
    static uint32_t VersionMinor();
    static uint32_t VersionPatch();
};

}  // namespace rt