#include <iostream>
#include <unistd.h>

int main(int argc, char** argv) {
	// Extract number of frames from the argument
	if (argc != 2) {
		std::cout << "Please provide the number of frames as a parameter" << std::endl;
		return 1;
	}
	int numframes = std::atoi(argv[1]);
	
	// Loop through all frames
	for (int frame = 0; frame < numframes; frame++) {
		// Wait 5 milliseconds
		usleep(5000);
		
		// Output the filename
		std::cout << "out_" << frame << ".csv" << std::endl;
	}
}