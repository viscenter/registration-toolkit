#include "manipulator.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

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
	if (file_path != "") 
	{
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
		success = true;
	}
	return success;
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