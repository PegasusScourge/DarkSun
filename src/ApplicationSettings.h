#pragma once
/**

File: ApplicationSettings.h
Description:

Stores settings for the application in a central place

*/

namespace darksun {

	class ApplicationSettings {

	public:
		ApplicationSettings();
		
		int opengl_depthBits;
		int opengl_stencilBits;
		int opengl_antialiasingLevel;
		int opengl_majorVersion;
		int opengl_minorVersion;
		bool opengl_vsync = false;
		int opengl_framerateLimit = 60;
		bool opengl_shadows = true;

	private:
	};

}
