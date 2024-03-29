#pragma once
/**

File: UiHandler.hpp
Description:

Header file for UiHandler.cpp, wrangles the UI for scenes (with LuaEngine for UI handling)

*/

#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
//#include "Renderer.hpp"
#include "ApplicationSettings.hpp"
#include "Camera.hpp"
#include "LuaEngine.hpp"
#include <mutex>

namespace darksun {

	struct WidgetData {
		string widgetName;
	};

	class UIWrangler {

	private:
		//std::shared_ptr<Renderer> renderer;
		std::shared_ptr<Camera> cam;

		std::mutex gui_mutex;
		std::unique_ptr<tgui::Gui> gui;

		LuaEngine uiEngine;

		string uiName = "";
		string lua_callbackTable = "FunctionCallbacks";

		bool shdTransition = false;

		// Hook the UI into the lua engine
		void hookUIInterface(ApplicationSettings* settings);

		/* LUA Functions */

		void transitionScene() { shdTransition = true; }

		// Callback function
		void callbackFunc(tgui::Widget::Ptr widget, const std::string& signalName);

		// Add a callback
		void registerWidgetCallback(string n, string t);

		bool isValidWidget(tgui::Widget::Ptr w) {
			bool valid = (w != nullptr);
			if (!valid)
				dlua.error("Attempted to access non-existant widget");
			return valid;
		}

		float getCameraX() {
			return cam->getGroundPosition().x;
		}
		float getCameraZ() {
			return cam->getGroundPosition().z;
		}

	public:
		UIWrangler(sf::RenderWindow* windowHandle, std::shared_ptr<Camera> cam, ApplicationSettings* settings, string uN);
		~UIWrangler();

		// Tick the engine and the ui for events etc
		void tick(float deltaTime);

		// Draw the UI
		void draw();

		// Handle events
		void handleEvent(sf::Event& ev);

		// Expose the engine
		LuaEngine* getUiEngine() { return &uiEngine; }

		// On create function needs to be called to init the lua!
		void OnCreate();

		// Get the scene to transition to
		string getNewScene() {
			lua::State *L = uiEngine.getState();
			try {
				LuaRef sceneTable = getGlobal(L->getState(), uiName.c_str());
				if (!sceneTable.isTable()) // Check to see if the scene table exists
					throw new std::exception("Scene table global not found");

				// Find the callback function we wantin the FunctionCallbacks table
				LuaRef transTable = sceneTable["SceneTransition"];
				if (!transTable.isTable())
					throw new std::exception("SceneTransition table not found");

				LuaRef v = transTable["To"];
				if (!v.isString())
					throw new std::exception("Couldn't find 'to' value of table");
				return v.tostring();
			}
			catch (std::exception& e) {
				string what = e.what();
				dlua.error("Failed Scene Transition info get (" + uiName + "): " + what);
			}
			return "";
		}

		// Should we initiate a transition
		bool shouldTransition() {
			return shdTransition;
		}

		/* LUA FUNCTIONS */

		// Add the widget to the current active parent
		void addNewWidget(tgui::Widget::Ptr widget, string n) {
			std::lock_guard lock(gui_mutex);
			WidgetData dat;
			dat.widgetName = n;
			widget->setUserData(dat);
			//widget->setSize("2%", "2%");
			gui->add(widget, n);
		}

		// Set the callback table
		void setCallbackTable(string n) { lua_callbackTable = n; }

		void addNewChatBox(string n) { tgui::ChatBox::Ptr w = tgui::ChatBox::create(); addNewWidget(w, n); }
		void addNewButton(string n) { tgui::Button::Ptr w = tgui::Button::create(); addNewWidget(w, n); }
		void addNewEditBox(string n) { tgui::EditBox::Ptr w = tgui::EditBox::create(); addNewWidget(w, n); }
		void addNewLabel(string n) { tgui::Label::Ptr w = tgui::Label::create(); addNewWidget(w, n); }
		void addNewPicture(string n, tgui::Texture& tex, bool transparentTex = false) { tgui::Picture::Ptr w = tgui::Picture::create(tex, transparentTex); addNewWidget(w, n); }
		void addNewProgressBar(string n) { tgui::ProgressBar::Ptr w = tgui::ProgressBar::create(); addNewWidget(w, n); }
		void addNewRadioButton(string n) { tgui::RadioButton::Ptr w = tgui::RadioButton::create(); addNewWidget(w, n); }
		void addNewCheckBox(string n) { tgui::CheckBox::Ptr w = tgui::CheckBox::create(); addNewWidget(w, n); }
		void addNewSpinButton(string n) { tgui::SpinButton::Ptr w = tgui::SpinButton::create(); addNewWidget(w, n); }
		void addNewComboBox(string n) { tgui::ComboBox::Ptr w = tgui::ComboBox::create(); addNewWidget(w, n); }
		void addNewKnob(string n) { tgui::Knob::Ptr w = tgui::Knob::create(); addNewWidget(w, n); }
		void addNewListBox(string n) { tgui::ListBox::Ptr w = tgui::ListBox::create(); addNewWidget(w, n); }
		void addNewListView(string n) { tgui::ListView::Ptr w = tgui::ListView::create(); addNewWidget(w, n); }
		void addNewMenuBar(string n) { tgui::MenuBar::Ptr w = tgui::MenuBar::create(); addNewWidget(w, n); }
		void addNewRangeSlider(string n) { tgui::RangeSlider::Ptr w = tgui::RangeSlider::create(); addNewWidget(w, n); }
		// tgui::Scrollbar skipped
		void addNewSlider(string n) { tgui::Slider::Ptr w = tgui::Slider::create(); addNewWidget(w, n); }
		void addNewTabs(string n) { tgui::Tabs::Ptr w = tgui::Tabs::create(); addNewWidget(w, n); }
		void addNewTextBox(string n) { tgui::TextBox::Ptr w = tgui::TextBox::create(); addNewWidget(w, n); }
		void addNewTreeView(string n) { tgui::TreeView::Ptr w = tgui::TreeView::create(); addNewWidget(w, n); }

		// Add container stuff (tgui::Container)

		// Set the size of a widget (in percentage)
		void setWidgetSizePercent(string n, string x, string y) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return; }
			w->setSize(x, y);
		}
		// Set the size of a widget (absolute)
		void setWidgetSize(string n, int x, int y) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return; }
			w->setSize(x, y);
		}
		// Set the position of a widget (in percentage)
		void setWidgetPositionPercent(string n, string x, string y) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return; }
			w->setPosition(x, y);
		}
		// Set the position of a widget (absolute)
		void setWidgetPosition(string n, int x, int y) {
			std::lock_guard lock(gui_mutex);
			tgui::Widget::Ptr w = gui->get(n);

			w->setPosition(x, y);
		}
		// Set the text of a button widget
		void setButtonWidgetText(string n, string text) {
			std::lock_guard lock(gui_mutex);
			tgui::Button::Ptr w = gui->get<tgui::Button>(n);
			if (!isValidWidget(w)) { return; }
			w->setText(text);
		}
		// Set a checkbox widget as checked
		void setCheckBoxWidgetChecked(string n, bool t) {
			std::lock_guard lock(gui_mutex);
			tgui::CheckBox::Ptr w = gui->get<tgui::CheckBox>(n);
			if (!isValidWidget(w)) { return; }
			w->setChecked(t);
		}
		// Set a checkbox widget text
		void setCheckBoxWidgetText(string n, string text) {
			std::lock_guard lock(gui_mutex);
			tgui::CheckBox::Ptr w = gui->get<tgui::CheckBox>(n);
			if (!isValidWidget(w)) { ; return; }
			w->setText(text);
		}
		// Set the text of a label widget
		void setLabelWidgetText(string n, string text) {
			std::lock_guard lock(gui_mutex);
			tgui::Label::Ptr w = gui->get<tgui::Label>(n);
			if (!isValidWidget(w)) { ; return; }
			w->setText(text);
		}
		// Set the widget visible
		void setWidgetVisible(string n, bool v) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return; }
			w->setVisible(v);
		}
		// Set the widget enabled
		void setWidgetEnabled(string n, bool v) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return; }
			w->setEnabled(v);
		}
		// is the widget visible
		bool isWidgetVisible(string n) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return true; }
			return w->isVisible();
		}
		// is the widget enabled
		bool isWidgetEnabled(string n) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return true; }
			return w->isEnabled();
		}
		// Show with effect the widget
		void showWithEffect(string n, string eff, int interval);
		// Hide with effect
		void hideWithEffect(string n, string eff, int interval);

		tgui::Widget::Ptr getWidgetByName(string n) {
			std::lock_guard lock(gui_mutex);
			auto w = gui->get(n);
			if (!isValidWidget(w)) { return NULL; }
			return w;
		}

	};

}
