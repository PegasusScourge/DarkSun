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
					.addFunction("setWidgetSizePercent", &darksun::UIWrangler::setWidgetSizePercent)
					.addFunction("setWidgetSize", &darksun::UIWrangler::setWidgetSize)
					.addFunction("setWidgetPositionPercent", &darksun::UIWrangler::setWidgetPositionPercent)
					.addFunction("setWidgetPosition", &darksun::UIWrangler::setWidgetPosition)
					.addFunction("setButtonWidgetText", &darksun::UIWrangler::setButtonWidgetText)
					.addFunction("setLabelWidgetText", &darksun::UIWrangler::setLabelWidgetText)
					.addFunction("registerWidgetCallback", &darksun::UIWrangler::registerWidgetCallback)
					.addFunction("setCallbackTable", &darksun::UIWrangler::setCallbackTable)
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
			throw new std::exception("OnTick function not found/not a function");
		onTick(deltaTime);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI OnTick (" + uiName + "): " + what);
	}
}

void UIWrangler::registerWidgetCallback(string n, string t) {
	lua::State *L = uiEngine.getState();
	try {
		auto w = gui->get(n);
		if (w == nullptr) {// Attempted to get non-existant widget
			throw new std::exception("Attempted to access non-existant widget");
		}
		w->connect(t.c_str(), &darksun::UIWrangler::callbackFunc, this);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI registerCallback (" + uiName + "): " + what);
	}
}

void UIWrangler::callbackFunc(tgui::Widget::Ptr widget, const std::string& signalName) {
	lua::State *L = uiEngine.getState();
	try {
		LuaRef sceneTable = getGlobal(L->getState(), uiName.c_str());
		if (!sceneTable.isTable()) // Check to see if the scene table exists
			throw new std::exception("Scene table global not found");

		// Find the callback function we wantin the FunctionCallbacks table
		LuaRef callbackTable = sceneTable["FunctionCallbacks"];
		if (!callbackTable.isTable())
			throw new std::exception("FunctionCallbacks table not found");

		string widgetName = widget->getUserData<string>();
		LuaRef func = callbackTable[widgetName.c_str()];

		if (!func.isFunction())
			throw new std::exception("Function not found/not a function");
		dlua.verbose("Callback: f=" + func.tostring() + ", widget=" + widgetName);
		func(widgetName);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI callback (" + uiName + "): " + what);
	}
}
