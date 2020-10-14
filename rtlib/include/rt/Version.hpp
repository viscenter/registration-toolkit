#pragma once

/** @file */

#include <string>

namespace rt
{

/** @brief Provides programmatic access to codebase information */
struct ProjectInfo {
    /** @brief Get the library name */
    static std::string Name();
    /** @brief Get the library version as a Major.Minor.Patch string */
    static std::string VersionString();
    /** @brief Get the library name and version string */
    static std::string NameAndVersion();
    /** @brief Get the library Major version number */
    static uint32_t VersionMajor();
    /** @brief Get the library Minor version number */
    static uint32_t VersionMinor();
    /** @brief Get the library Patch version number */
    static uint32_t VersionPatch();
    /** @brief Get the git repository URL */
    static std::string RepositoryURL();
    /** @brief Get the full hash for the current git commit */
    static std::string RepositoryHash();
    /** @brief Get the short hash for the current git commit */
    static std::string RepositoryShortHash();
};

}  // namespace rt