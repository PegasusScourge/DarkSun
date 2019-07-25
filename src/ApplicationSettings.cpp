/**

File: ApplicationSettings.cpp
Description:

Stores settings for the application in a central place

*/

#include "ApplicationSettings.h"

using namespace darksun;

ApplicationSettings::ApplicationSettings(string settingsFile) {

	// Init the opengl context settings
	opengl_depthBits = 24;
	opengl_stencilBits = 8;
	opengl_antialiasingLevel = 4;
	opengl_majorVersion = 3;
	opengl_minorVersion = 3;
	opengl_vsync = false;

	if (!engine.isValid()) {
		dout.error("Application settings LuaEngine is invalid??");
	}

	// Load the settings
	loadSettings(settingsFile);

}

void ApplicationSettings::loadSettings(string settings) {
	dout.log("Settings --> Loading settings...");

	// Add the file containing the settings
	engine.addFile(settings);
	
	lua::State *L = engine.getState();
	try {
		
		LuaRef settingsTable = getGlobal(L->getState(), "settings");
		if (!settingsTable.isTable()) {
			dout.error("Settings --> No settings table is present in file '" + settings + "'");
			return;
		}

		LuaRef graphicsTable = settingsTable["graphics"];
		if (graphicsTable.isTable()) {
			// We have graphics settings

			int antiAlias = engine.getInt(graphicsTable, "antialiasing_level");
			if (antiAlias >= 0 && antiAlias <= 8 && antiAlias % 2 == 0) {
				opengl_antialiasingLevel = antiAlias;
				dout.log("Settings --> graphics.antialiasing_level = '" + std::to_string(antiAlias) + "'");
			}
		}

	}
	catch (std::exception& e) {
		string what = e.what();
		dout.error("Failed to load the application settings: " + what);
	}

	dout.log("Settings --> Loaded!");
}