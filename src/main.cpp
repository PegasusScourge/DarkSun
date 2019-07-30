/**

File: main.cpp
Description:

Entry point of the application (main())

*/

// Needed first for std_image.hpp so it behaves correctly
#define STB_IMAGE_IMPLEMENTATION

#include "DarkSun.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
	// Redirec the cout
	//std::ofstream out("cout.log");
	//std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
	//std::cout.rdbuf(out.rdbuf()); //redirect std::cout
	
	darksun::DarkSun engine;

	engine.processArgs(argc, argv);

	engine.run();

	return 0;
}