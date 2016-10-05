#include "manipulator.hpp"

int main(int argc, char* argv[])
{
	// Parse command line arguments
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " Filename(.obj)" << std::endl;
		return EXIT_FAILURE;
	}

	std::string file_path = argv[1];
	Manipulator obj_manipulator = Manipulator(file_path);
	if (!obj_manipulator.ManipulateObj())
	{
		return EXIT_FAILURE;
	}
	
	return 0;
}


