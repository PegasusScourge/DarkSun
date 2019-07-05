/**

File: main.cpp
Description:

Entry point of the application (main())

*/

// Needed first for std_image.h so it behaves correctly
#define STB_IMAGE_IMPLEMENTATION

#include "Log.h"
#include "DarkSun.h"

using namespace darksun;

int main(int argc, char *argv[]) {
	DarkSun engine;

	engine.processArgs(argc, argv);

	engine.run();

	return 0;
}