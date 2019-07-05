#pragma once
/**

File: DarkSun.h
Description:

Main app

*/

#include "Renderer.h"
#include "LuaEngine.h"
#include "Log.h"

#include <SFML/Graphics.hpp>

using namespace darksun;

namespace darksun {

	class DarkSun {

	private:


	public:
		/* Default constructor */
		DarkSun();

		/* Processes the arguments from the command line */
		void processArgs(int argc, char *argv[]);

		/* Does the execution */
		void run();

	};

}