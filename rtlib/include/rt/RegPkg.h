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

        RegPkg(string root_dir); //Constructor with setting root directory, linking to a regpkg

        auto getInputs() const }; //Return list of inputs

        auto getInput(string input_dir) const {}; //Get one of the input data objects from the existing list

        auto addInput(string input_dir) {};

        auto removeInput(string input_dir) {};

        auto loadRegPkg(string root_dir) {};


    private:
        string root_dir; //root directory of this regpkg
        auto inputDataList; //some list of all the input data objects. What are those object types?
        void save() {};
};
}
