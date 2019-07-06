#pragma once
/**

File: UiHandler.h
Description:

Header file for UiHandler.cpp, wrangles the UI for scenes (with LuaEngine for UI handling)

*/

#include <TGUI/TGUI.hpp>
#include "Renderer.h"
#include "LuaEngine.h"

namespace darksun {

	class UIWrangler {

	private:
		std::shared_ptr<Renderer> renderer;

		std::unique_ptr<tgui::Gui> gui;

		LuaEngine uiEngine;

		string uiName = "";

	public:
		UIWrangler(std::shared_ptr<Renderer> r, string uN);
		~UIWrangler();

		// Tick the engine and the ui for events etc
		void tick();

		// Draw the UI
		void draw();

		// Handle events
		void handleEvent(sf::Event& ev);

	};

}
