#pragma once

#include <string>

namespace rt
{

/** Provides programmatic access to codebase information */
struct ProjectInfo {
    /** Get the library name */
    static std::string Name();
    /** Get the library version Major.Minor.Patch string */
    static std::string VersionString();
    /** Get the library name and full version */
    static std::string NameAndVersion();
    /** Get the library Major version number */
    static uint32_t VersionMajor();
    /** Get the library Minor version number */
    static uint32_t VersionMinor();
    /** Get the library Patch version number */
    static uint32_t VersionPatch();
    /** Get the full hash for the current git commit */
    static std::string GitHash();
    /** Get the seven digit short short for the current git commit */
    static std::string GitHashShort();
};

}  // namespace rt