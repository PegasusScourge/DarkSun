/**

File: UiHandler.cpp
Description:

Wrangles the UI for scenes (with LuaEngine for UI handling)

*/
#include "UiHandler.h"

using namespace darksun;

UIWrangler::UIWrangler(sf::RenderWindow* windowHandle, std::shared_ptr<Camera> cam, ApplicationSettings* settings, string uN) : cam(cam), uiName(uN) {
	dout.log("[ Init UI : " + uiName + " ]");
	dout.log("Loading the LuaEngine");

	uiEngine.addFilesRecursive("lua/ui/" + uiName + "/", ".lua");
	dout.log("Added ui files to the engine");

	gui = std::unique_ptr<tgui::Gui>(new tgui::Gui(*windowHandle));

	hookUIInterface(settings);

	dout.log("[ UI DONE : " + uiName + " ]");
}

void UIWrangler::OnCreate() {
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

void UIWrangler::hookUIInterface(ApplicationSettings* settings) {
	lua::State *L = uiEngine.getState();

	try {
		luabridge::getGlobalNamespace(L->getState())
			.beginNamespace("darksun")
				.beginClass<UIWrangler>("GUI")
					//.addConstructor<void(*)(std::shared_ptr<Renderer> r, ApplicationSettings& settings, string uN), RefCountedPtr<UIWrangler> /* creation policy */ >()
					.addProperty("name", &darksun::UIWrangler::uiName, false) // Read only
					.addFunction("cameraX", &darksun::UIWrangler::getCameraX) // Read only
					.addFunction("cameraZ", &darksun::UIWrangler::getCameraZ) // Read only
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
					.addFunction("setCheckBoxWidgetChecked", &darksun::UIWrangler::setCheckBoxWidgetChecked)
					.addFunction("setCheckBoxWidgetText", &darksun::UIWrangler::setCheckBoxWidgetText)
					.addFunction("registerWidgetCallback", &darksun::UIWrangler::registerWidgetCallback)
					.addFunction("setCallbackTable", &darksun::UIWrangler::setCallbackTable)
					.addFunction("setWidgetVisible", &darksun::UIWrangler::setWidgetVisible)
					.addFunction("setWidgetEnabled", &darksun::UIWrangler::setWidgetEnabled)
					.addFunction("isWidgetEnabled", &darksun::UIWrangler::isWidgetEnabled)
					.addFunction("isWidgetVisible", &darksun::UIWrangler::isWidgetVisible)
					.addFunction("showWithEffect", &darksun::UIWrangler::showWithEffect)
					.addFunction("hideWithEffect", &darksun::UIWrangler::hideWithEffect)
					.addFunction("transitionScene", &darksun::UIWrangler::transitionScene)
				.endClass()
				.beginClass<ApplicationSettings>("ApplicationSettings")
					//.addConstructor<void(*)(), RefCountedPtr<ApplicationSettings> /* creation policy */ >()
					.addFunction("opengl_depthBits", &darksun::ApplicationSettings::get_opengl_depthBits)
					.addFunction("opengl_stencilBits", &darksun::ApplicationSettings::get_opengl_stencilBits)
					.addFunction("opengl_antialiasingLevel", &darksun::ApplicationSettings::get_opengl_antialiasingLevel)
					.addFunction("opengl_majorVersion", &darksun::ApplicationSettings::get_opengl_majorVersion)
					.addFunction("opengl_minorVersion", &darksun::ApplicationSettings::get_opengl_minorVersion)
					.addFunction("opengl_vsync", &darksun::ApplicationSettings::get_opengl_vsync)
					.addFunction("set_opengl_vsync", &darksun::ApplicationSettings::set_opengl_vsync)
					.addFunction("opengl_framerateLimit", &darksun::ApplicationSettings::get_opengl_framerateLimit)
					.addFunction("set_opengl_framerateLimit", &darksun::ApplicationSettings::set_opengl_framerateLimit)
				.endClass()
			.endNamespace();

		// Add this instance
		push(L->getState(), this);
		lua_setglobal(L->getState(), "Gui");

		// Add the application settings
		push(L->getState(), settings);
		lua_setglobal(L->getState(), "Settings");
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
		LuaRef callbackTable = sceneTable[lua_callbackTable];
		if (!callbackTable.isTable())
			throw new std::exception("FunctionCallbacks table not found");

		WidgetData dat = widget->getUserData<WidgetData>();
		string widgetName = dat.widgetName;
		LuaRef func = callbackTable[widgetName.c_str()];

		if (!func.isFunction())
			throw new std::exception("Function not found/not a function");
		dlua.verbose("Callback: f=" + func.tostring() + ", widget=" + widgetName);
		func(signalName);
	}
	catch (std::exception& e) {
		string what = e.what();
		dlua.error("Failed UI callback (" + uiName + "): " + what);
	}
}

void UIWrangler::showWithEffect(string n, string eff, int interval) {
	auto w = gui->get(n);
	if (!isValidWidget(w)) { return; }
	string effect = eff;
	std::transform(eff.begin(), eff.end(), effect.begin(), ::tolower); // Convert string to lowercase

	if (effect.compare("fade") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(interval));
	}
	else if (effect.compare("scale") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::Scale, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefrombottom") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideFromBottom, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromleft") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideFromLeft, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromright") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideFromRight, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromtop") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideFromTop, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetobottom") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideToBottom, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetoleft") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideToLeft, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetoright") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideToRight, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetotop") == 0) {
		w->showWithEffect(tgui::ShowAnimationType::SlideToTop, sf::milliseconds(interval));
	}
	else {
		dlua.error("Attempted unknown showWithEffect: '" + effect + "'");
		w->setVisible(true);
	}
}

void UIWrangler::hideWithEffect(string n, string eff, int interval) {
	auto w = gui->get(n);
	if (!isValidWidget(w)) { return; }
	string effect = eff;
	std::transform(eff.begin(), eff.end(), effect.begin(), ::tolower); // Convert string to lowercase

	if (effect.compare("fade") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::Fade, sf::milliseconds(interval));
	}
	else if (effect.compare("scale") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::Scale, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefrombottom") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideFromBottom, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromleft") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideFromLeft, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromright") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideFromRight, sf::milliseconds(interval));
	}
	else if (effect.compare("slidefromtop") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideFromTop, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetobottom") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideToBottom, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetoleft") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideToLeft, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetoright") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideToRight, sf::milliseconds(interval));
	}
	else if (effect.compare("slidetotop") == 0) {
		w->hideWithEffect(tgui::ShowAnimationType::SlideToTop, sf::milliseconds(interval));
	}
	else {
		dlua.error("Attempted unknown hideWithEffect: '" + effect + "'");
		w->setVisible(false);
	}
}
