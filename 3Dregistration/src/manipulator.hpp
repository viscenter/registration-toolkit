#ifndef MESH_MANIPULATOR_H_
#define MESH_MANIPULATOR_H_

#include <stdbool.h>
#include <string>
#include <vtkSmartPointer.h>
#include <vtkOBJReader.h>

class Manipulator 
{
	public:
		Manipulator();
		Manipulator(std::string &file);
		bool ManipulateObj();
		void Visualize();

		// Getters and setters
		void SetObjFilePath(std::string &file);
		std::string GetObjFilePath();

	private:
		bool ReadObj();
		void Normalize(double *values);

		// Private data members
		std::string file_path;
		vtkSmartPointer<vtkOBJReader> reader;
};
#endif //MESH_MANIPULATOR_H_