/**

File: UiHandler.cpp
Description:

Wrangles the UI for scenes (with LuaEngine for UI handling)

*/
#include "UiHandler.h"

using namespace darksun;

UIWrangler::UIWrangler(std::shared_ptr<Renderer> r, string uN) : renderer(r), uiName(uN) {
	dout.log("[ Init UI : " + uiName + " ]");
	dout.log("Loading the LuaEngine");

	uiEngine.addFilesRecursive("lua/ui/", ".lua");
	dout.log("Added ui files to the engine");

	gui = std::unique_ptr<tgui::Gui>(new tgui::Gui(*renderer->getWindowHandle()));

	dout.log("[ UI DONE : " + uiName + " ]");
}

void UIWrangler::draw() {
	gui->draw();
}

void UIWrangler::handleEvent(sf::Event& ev) {
	gui->handleEvent(ev);
}

UIWrangler::~UIWrangler() {
	gui->removeAllWidgets();
	gui.release();
}

void UIWrangler::tick() {
	uiEngine.tick();
}
