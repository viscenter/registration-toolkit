#pragma once

/** @file */

#include <regex>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

namespace rt
{
/** List of file extensions type */
using ExtensionList = std::vector<std::string>;

/** Filter a path by a list of approved file extensions */
inline bool FileExtensionFilter(
    const boost::filesystem::path& path, const ExtensionList& exts)
{
    std::string regexExpression = ".*\\.(";
    size_t count = 0;
    for (const auto& e : exts) {
        regexExpression.append(e);
        if (++count < exts.size()) {
            regexExpression.append("|");
        }
    }
    regexExpression.append(")$");

    std::regex extensions{regexExpression, std::regex::icase};
    return std::regex_match(path.extension().string(), extensions);
}
}  // namespace rt
