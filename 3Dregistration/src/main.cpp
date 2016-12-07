#include "obj_reader.hpp"
#include "manipulator.hpp"

int main(int argc, char* argv[])
{
	// Parse command line argumentsx
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " Filename(.obj)" << std::endl;
		return EXIT_FAILURE;
	}

	std::string obj_file = argv[1];
	Manipulator m = Manipulator(obj_file);
	m.ManipulateObj();
	return 0;
}


