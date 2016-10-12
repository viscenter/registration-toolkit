#include "manipulator.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOBBTree.h>
#include <vtkRenderWindowInteractor.h>
#include <math.h>
#include <limits.h>

Manipulator::Manipulator() 
{
	file_path = "";
	reader = vtkSmartPointer<vtkOBJReader>::New();
}

Manipulator::Manipulator(std::string &file) 
{
	file_path = file;
	reader = vtkSmartPointer<vtkOBJReader>::New();
}

bool Manipulator::ReadObj()
{
	bool success = false;
	// Check to see if the Manipulator has been initialized with a file path
	if (file_path != "") 
	{
		// Set the file path for the reader and get the mesh (obj)
		reader->SetFileName(file_path.c_str()); 
		reader->Update();
		success = true;
	}
	return success;
}

bool Manipulator::ManipulateObj()
{
	bool success = false;
	if (ReadObj() && reader->GetFileName() != NULL) 
	{
		vtkSmartPointer<vtkPolyData> poly_data = reader->GetOutput();
		vtkSmartPointer<vtkOBBTree> obb_tree = vtkSmartPointer<vtkOBBTree>::New();
		double corner[3]; double max[3]; double mid[3]; double min[3]; double size[3];
		// Computes the OBB and returns the 3 axis relative to the box
		obb_tree->ComputeOBB(poly_data, corner, max, mid, min, size);

		// Convert to points of space; We assume that the min array is the "z axis"
		for (int i = 0; i < 3; ++i) 
		{
			min[i] += corner[i];
		}
		// Normalize the vector
		Normalize(min);
		//AlignObj(reader-);
		success = true;
	}
	return success;
}



void Manipulator::Normalize(double *values)
{
	double magnitude = 0.0;
	for (int i = 0; i < 3; ++i) 
	{
		magnitude += (values[i] * values[i]);
	}
	magnitude = sqrt(magnitude);
	if (magnitude > 0.0) 
	{
		for (int i = 0; i < 3; ++i) 
		{
			values[i] = values[i]/magnitude;
		}
	}
	return;
}

void Manipulator::Visualize()
{
	//Visualize the data
	vtkSmartPointer<vtkPolyDataMapper> mapper =
	vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
	vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer =
	vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(.3, .3, .6); // Background color blue

	vtkSmartPointer<vtkRenderWindow> renderWindow =
	vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderWindowInteractor->Start();
}

void Manipulator::SetObjFilePath(std::string &file)
{
	file_path = file;
}


std::string Manipulator::GetObjFilePath()
{
	return file_path;
}