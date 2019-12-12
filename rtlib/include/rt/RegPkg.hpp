//
// Created by Anthony Tamasi on 2019-10-10.
//

#pragma once

//Standard library
#include <string>

namespace rt {
/**
 * @class RegPkg
 * @brief Interface for an organization structure for image
 * registration projects
 * @ingroup regpkg
 */
class RegPkg
{

public:
    /** Constructor */
    RegPkg();
    /** Constructor overload */
    RegPkg(std::string root_dir);  // Constructor with setting root directory,
                                   // linking to a regpkg

    /** @brief Return list of data inputs */
    auto getInputs() const {};  // Return list of inputs. What should the type
                                // be?? General data class????

    /** @brief Return a specific data input */
    auto getInput(std::string input_dir)
        const {};  // Get one of the input data objects from the existing list

    /** @brief Add a data input */
    auto addInput(std::string input_dir){};

    /** @brief Remove a data input from the list */
    auto removeInput(std::string input_dir){};

    /** @brief Load in a registration package from a file directory */
    auto loadRegPkg(std::string root_dir){};

private:
    /**  Root directory of the registration package*/
    std::string rootPath_;

    /** List of all input data objects this registration package contains */
    auto inputDataList_;

    /** Save this registration package */
    void save(){};
};
}
