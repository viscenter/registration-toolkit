#include "obj_reader.hpp"

int main(int argc, char* argv[])
{
	// Parse command line arguments
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " Filename(.obj)" << std::endl;
		return EXIT_FAILURE;
	}

	std::string obj_file = argv[1];
	OBJReader reader = OBJReader(); 
	reader.Parse(obj_file);
	return 0;
}


