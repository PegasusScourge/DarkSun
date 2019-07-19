/**

File: ApplicationSettings.cpp
Description:

Stores settings for the application in a central place

*/

#include "ApplicationSettings.h"

using namespace darksun;

ApplicationSettings::ApplicationSettings() {

	// Init the opengl context settings
	opengl_depthBits = 24;
	opengl_stencilBits = 8;
	opengl_antialiasingLevel = 4;
	opengl_majorVersion = 3;
	opengl_minorVersion = 3;
	opengl_vsync = false;

}