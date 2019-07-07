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

		// Hook the UI into the lua engine
		void hookUIInterface();

		/* LUA EXPOSED FUNCTIONS FOR UI CREATION ETC */
		
		// Add the widget to the current active parent
		void addNewWidget(tgui::Widget::Ptr widget, string n) {
			gui->add(widget, n);
		}

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
