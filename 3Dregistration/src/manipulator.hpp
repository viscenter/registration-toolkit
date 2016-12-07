#ifndef MESH_MANIPULATOR_H_
#define MESH_MANIPULATOR_H_

#include "obj_reader.hpp"

#include <stdbool.h>
#include <string>
#include <vtkSmartPointer.h>
#include <vtkOBJReader.h>
#include <vtkTransformPolyDataFilter.h>

using Element = double;
const size_t Dimension = 3;

class Manipulator 
{
	public:
		Manipulator();
		Manipulator(std::string &file);
		bool ManipulateObj();
		void VisualizeResults(vtkSmartPointer<vtkOBJReader> reader);

		// Getters and setters
		void SetObjFilePath(std::string &file);
		std::string GetObjFilePath();

	private:
		bool ReadObj(vtkSmartPointer<vtkOBJReader> reader);
		vtkSmartPointer<vtkPolyData> AlignObj(vtkSmartPointer<vtkPolyData> data);
		void WriteObj(vtkSmartPointer<vtkPolyData> mesh);
		vtkSmartPointer<vtkPolyData> FindIntersections(vtkSmartPointer<vtkPolyData> aligned_data);
		cv::Vec3d BarycentricCoord(cv::Vec3d nXYZ, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC);
		cv::Vec3d CartesianCoord(cv::Vec3d nUVW, cv::Vec3d nA, cv::Vec3d nB, cv::Vec3d nC);

		// Private data members
		std::string _file_path;
		OBJReader _uv_reader;
		cv::Mat _texture_img;
};
#endif //MESH_MANIPULATOR_H_
