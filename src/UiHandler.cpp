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

	uiEngine.addFilesRecursive("lua/ui/" + uiName + "/", ".lua");
	dout.log("Added ui files to the engine");

	gui = std::unique_ptr<tgui::Gui>(new tgui::Gui(*renderer->getWindowHandle()));

	hookUIInterface();

	dout.log("[ UI DONE : " + uiName + " ]");

	// Init the scene
	lua::State *L = uiEngine.getState();
	try {
		LuaRef sceneTable = getGlobal(L->getState(), uiName.c_str());
		if (!sceneTable.isTable()) // Check to see if the scene table exists
			throw new std::exception("Scene table global not found");

		//dlua.verbose("Got scene table");
		LuaRef onCreate = sceneTable["OnCreate"];
		if (!onCreate.isFunction())
			throw new std::exception("OnCreate function not found/mot a function");
		//dlua.verbose("Got onCreate");
		onCreate();
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI OnCreate (" + uiName + "): " + what);
	}
}

void UIWrangler::hookUIInterface() {
	lua::State *L = uiEngine.getState();

	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<UIWrangler>("GUI")
					.addConstructor<void(*)(std::shared_ptr<Renderer> r, string uN), RefCountedPtr<UIWrangler> /* creation policy */ >()
					.addProperty("name", &darksun::UIWrangler::uiName, false) // Read only
					.addFunction("addNewChatBox", &darksun::UIWrangler::addNewChatBox)
					.addFunction("addNewButton", &darksun::UIWrangler::addNewButton)
					.addFunction("addNewEditBox", &darksun::UIWrangler::addNewEditBox)
					.addFunction("addNewLabel", &darksun::UIWrangler::addNewLabel)
					.addFunction("addNewPicture", &darksun::UIWrangler::addNewPicture)
					.addFunction("addNewProgressBar", &darksun::UIWrangler::addNewProgressBar)
					.addFunction("addNewRadioButton", &darksun::UIWrangler::addNewRadioButton)
					.addFunction("addNewCheckBox", &darksun::UIWrangler::addNewCheckBox)
					.addFunction("addNewSpinButton", &darksun::UIWrangler::addNewSpinButton)
					.addFunction("addNewComboBox", &darksun::UIWrangler::addNewComboBox)
					.addFunction("addNewKnob", &darksun::UIWrangler::addNewKnob)
					.addFunction("addNewListBox", &darksun::UIWrangler::addNewListBox)
					.addFunction("addNewListView", &darksun::UIWrangler::addNewListView)
					.addFunction("addNewMenuBar", &darksun::UIWrangler::addNewMenuBar)
					.addFunction("addNewRangeSlider", &darksun::UIWrangler::addNewRangeSlider)
					.addFunction("addNewSlider", &darksun::UIWrangler::addNewSlider)
					.addFunction("addNewTabs", &darksun::UIWrangler::addNewTabs)
					.addFunction("addNewTextBox", &darksun::UIWrangler::addNewTextBox)
					.addFunction("addNewTreeView", &darksun::UIWrangler::addNewTreeView)
				.endClass()
			.endNamespace()
			.addFunction("LOG", lua_log);

		// Add this instance
		push(L->getState(), this);
		lua_setglobal(L->getState(), "myGui");
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI binding proccess: " + what);
		return;
	}
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

void UIWrangler::tick(float deltaTime) {
	lua::State *L = uiEngine.getState();
	try {
		LuaRef sceneTable = getGlobal(L->getState(), uiName.c_str());
		if (!sceneTable.isTable()) // Check to see if the scene table exists
			throw new std::exception("Scene table global not found");
		LuaRef onTick = sceneTable["OnTick"];
		if (!onTick.isFunction())
			throw new std::exception("OnTick function not found/mot a function");
		onTick(deltaTime);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI OnTick (" + uiName + "): " + what);
	}
}
