#pragma once
/**

File: DarkSun.h
Description:

Main app

*/

#include <filesystem>
#include <vector>

#include "Renderer.h"
#include "Log.h"

#include "Scene.h"

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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