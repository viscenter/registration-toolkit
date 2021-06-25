#pragma once

/** @file */

#include <string>

namespace rt
{

/** @brief Provides programmatic access to codebase information */
struct ProjectInfo {
    /** @brief Get the library name */
    static auto Name() -> std::string;
    /** @brief Get the library version as a Major.Minor.Patch string */
    static auto VersionString() -> std::string;
    /** @brief Get the library name and version string */
    static auto NameAndVersion() -> std::string;
    /** @brief Get the library Major version number */
    static auto VersionMajor() -> uint32_t;
    /** @brief Get the library Minor version number */
    static auto VersionMinor() -> uint32_t;
    /** @brief Get the library Patch version number */
    static auto VersionPatch() -> uint32_t;
    /** @brief Get the git repository URL */
    static auto RepositoryURL() -> std::string;
    /** @brief Get the full hash for the current git commit */
    static auto RepositoryHash() -> std::string;
    /** @brief Get the short hash for the current git commit */
    static auto RepositoryShortHash() -> std::string;
};

}  // namespace rt