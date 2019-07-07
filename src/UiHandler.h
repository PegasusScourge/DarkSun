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
		string lua_callbackTable = "FunctionCallbacks";

		// Hook the UI into the lua engine
		void hookUIInterface();

		/* LUA EXPOSED FUNCTIONS FOR UI CREATION ETC */
		
		// Callback function
		void callbackFunc(tgui::Widget::Ptr widget, const std::string& signalName);

		// Add a callback
		void registerWidgetCallback(string n, string t);

		// Add the widget to the current active parent
		void addNewWidget(tgui::Widget::Ptr widget, string n) {
			widget->setUserData(n);
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
			auto w = gui->get(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setSize(x, y);
		}
		// Set the size of a widget (absolute)
		void setWidgetSize(string n, int x, int y) {
			auto w = gui->get(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setSize(x, y);
		}
		// Set the position of a widget (in percentage)
		void setWidgetPositionPercent(string n, string x, string y) {
			auto w = gui->get(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setPosition(x, y);
		}
		// Set the position of a widget (absolute)
		void setWidgetPosition(string n, int x, int y) {
			auto w = gui->get(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setPosition(x, y);
		}
		// Set the text of a button widget
		void setButtonWidgetText(string n, string text) {
			tgui::Button::Ptr w = gui->get<tgui::Button>(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setText(text);
		}
		// Set the text of a label widget
		void setLabelWidgetText(string n, string text) {
			tgui::Label::Ptr w = gui->get<tgui::Label>(n);
			if (w == nullptr) {// Attempted to get non-existant widget
				dlua.error("Attempted to access non-existant widget");
				return;
			}
			w->setText(text);
		}

	public:
		UIWrangler(std::shared_ptr<Renderer> r, string uN);
		~UIWrangler();

		// Tick the engine and the ui for events etc
		void tick(float deltaTime);

		// Draw the UI
		void draw();

		// Handle events
		void handleEvent(sf::Event& ev);

	};

}
