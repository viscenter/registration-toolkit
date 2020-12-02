#pragma once

/** @file */

#ifdef RT_USE_BOOSTFS
#include <boost/filesystem.hpp>
namespace rt
{
/**
 * @namespace rt::filesystem
 * @brief Alias for `boost::filesystem`
 *
 * If CMake variable `RT_USE_BOOSTFS` is false, then `rt::filesystem` is an
 * alias for `std::filesystem`. Otherwise, it is an alias for
 * `boost::filesystem`. This enables filesystem compatibility for compilers
 * which do not provide `std::filesystem`.
 */
namespace filesystem = boost::filesystem;
}  // namespace rt
#else
#include <filesystem>
namespace rt
{
/**
 * @namespace rt::filesystem
 * @brief Alias for `std::filesystem`
 *
 * If CMake variable `RT_USE_BOOSTFS` is false, then `rt::filesystem` is an
 * alias for `std::filesystem`. Otherwise, it is an alias for
 * `boost::filesystem`. This enables filesystem compatibility for compilers
 * which do not provide `std::filesystem`.
 */
namespace filesystem = std::filesystem;
}  // namespace rt
#endif