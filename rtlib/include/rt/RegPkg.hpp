//
// Created by Anthony Tamasi on 2019-10-10.
//

#pragma once

//Standard library
#include <string>

namespace rt {
class RegPkg
{

    public:
        RegPkg(); //Default constructor

        RegPkg(std::string root_dir); //Constructor with setting root directory, linking to a regpkg

        auto getInputs() const }; //Return list of inputs. What should the type be?? General data class????

        auto getInput(std::string input_dir) const {}; //Get one of the input data objects from the existing list

        auto addInput(std::string input_dir) {};

        auto removeInput(std::string input_dir) {};

        auto loadRegPkg(std::string root_dir) {};


    private:
        std::string rootDir; //root directory of this regpkg
        auto inputDataList; //some list of all the input data objects. What are those object types?

        void save() {};
};
}
