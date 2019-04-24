//The infinitely-useful 'menu' class.
#pragma once

#ifndef __FGRMENU_H__
#define __FGRMENU_H__

#include "fgrdrawing.h"

#include <string>

namespace fgr {
	//The menu class provides a simple API for user interaction. Think of these
	//As very basic VB forms that use openGL
	class menu {
	public:
		//This type is used to represent submenu indices
		typedef int edex;
		//Directions as we know them
		enum direction { up, down, left, right };
	private:
		// REPRESENTATION
		//The graphic for this menu while it is a button
		graphic button_body;
		//The graphic for this menu while it is an open menu
		graphic menu_body;
		//Where it should appear while a button (relative to parent IF there is a parent, otherwise to the origin)
		point button_location;
		//Where is should appear while an open menu (relative to parent IF there is a parent, otherwise to the origin)
		point menu_location;
		//Scale it this way before rendering as a button
		point button_scale;
		//Scale it this way before rendering as a menu
		point menu_scale;
		//This function is called whenever a button in this menu is confirmed
		void (*child_activation_function)(edex);
		//Whether this is the current menu in focus (one per tree can be like this)
		bool focused;
		//Will not show unless this is true
		bool show_this;
		//False if this menu is never anything more than a button
		bool is_menu;

		// AS A PARENT MENU
		//IF this menu has any submenus(which behave as 'buttons'), they go in this vector (otherwise empty)
		std::vector<menu> elements;
		//Which submenu is currently selected (for directional key menu navigation)
		edex selectedElement;
		//Which should be considered the 'default' submenu for selection
		edex homeElement;

		//AS A CHILD MENU
		//Is this menu expanded?
		bool expanded;
		//The function called when this menu is selected (as if it is a button)
		void (*activation_function)();
		//The function called just before the button's graphic is drawn
		void (*pre_render_button_function)();
		//The function called just before the button's graphic is drawn
		void (*post_render_button_function)();
		//The function called just before the menu's graphic is drawn
		void (*pre_render_menu_function)();
		//The function called just before the menu's graphic is drawn
		void (*post_render_menu_function)();
		//Pointer to the menu this is nested in (also the parent menu)
		menu* nest;
		//This button must know its own index
		edex my_index;
		//The menu indices this button connects to, in the order up, down, left, right
		edex adjacencies[4];
	public:
		//(menu) Draw this menu, even if it has childeren currently being shown?
		bool draw_if_unfocused;
		//(child menu) Draw this menu, even if it isn't expanded?
		bool draw_if_unexpanded;
		void hide() {
			show_this = false;
		}
		//Default constructor
		menu() {
			my_index = 0;
			for (unsigned int a = 0; a < 4; ++a) {
				adjacencies[a] = my_index;
			}
			activation_function = nullptr;
			pre_render_button_function = nullptr;
			post_render_button_function = nullptr;
			pre_render_menu_function = nullptr;
			post_render_menu_function = nullptr;
			selectedElement = 0;
			homeElement = 0;
			nest = NULL;
			button_scale.x(1.0f);
			button_scale.y(1.0f);
			menu_scale.x(5.0f);
			menu_scale.y(5.0f);
			is_menu = false;
			expanded = false;
			draw_if_unexpanded = false;
			focused = false;
			draw_if_unfocused = false;
			show_this = true;
		}
		//Use this constructor if making a submenu who is just a button
		menu(const graphic& body, point location, point scale, edex* adjnums, void (*confunc)() = nullptr) : menu() {
			button_body = body;
			button_location = location;
			button_scale = scale;
			for (edex e = 0; e < 4; ++e) {
				adjacencies[e] = adjnums[e];
			}
			activation_function = confunc;
		}
		//More useful constructor, depends on the previous
		menu(const std::string& bodypath, point location, point scale, int buttonc, menu* buttonv, void (*choosefunc)(edex) = nullptr) : menu(){
			graphicFromFile(menu_body, bodypath);
			focused = true;
			expanded = true; //DELETE THIS LINE LATER
			is_menu = true;
			child_activation_function = choosefunc;
			menu_scale = scale;
			menu_location = location;
			for (int i = 0; i < buttonc; ++i) {
				elements.push_back(buttonv[i]);
				elements.back().nest = this;
				elements.back().my_index = elements.size() - 1;
			}
		}
		//Navigate in a particular direction enumerated { UP, DOWN, LEFT, RIGHT }
		void move(direction dirNum) {
			selectedElement = elements[selectedElement].adjacencies[dirNum];
		}
		//Some user-friendly navigation functions
		//Navigate UPWARDS in the menu
		void moveUp() {
			move(up);
		}
		//Navigate DOWNWARDS in the menu
		void moveDown() {
			move(down);
		}
		//Navigate LEFTWARDS in the menu
		void moveLeft() {
			move(left);
		}
		//Navigate RIGHTWARDS in the menu
		void moveRight() {
			move(right);
		}
		//'activate' a particular element
		void confirm_element(edex elementID) {
			elements[elementID].activation_function();
		}
		//'activate' the selected element
		void confirm_selected() {
			confirm_element(selectedElement);
		}
		//'activate' the selected element and return its ID
		edex confirmget() {
			confirm_selected();
			return selectedElement;
		}
		//Send a 'click' to this menu, who will activate the appropriate button if there is one
		edex sendclick(const point& location, bool allow_multiple = false) {
			for (edex e = 0; e < elements.size(); ++e) {
				if (elements[e].button_body.bounds().contains(location)) {
					confirm_element(e);
					selectedElement = e;
					return e;
				}
			}
			return -1;
		}
		//Send a 'hover' to this menu, who will focus the appropriate button if there is one
		edex sendhover(const point& location) {

		}

		//TODO: Copy constructor

		friend void draw(const menu& obj);
	};

	void draw(const menu& obj) {
		if (!obj.show_this)
			return;
		//First draw its button form IF it has one
		if (obj.nest) {
			glPushMatrix();
				glTranslatePoint(obj.button_location);
				glScalePoint(obj.button_scale);
				if (obj.pre_render_button_function)
					obj.pre_render_button_function();
				draw(obj.button_body);
				if (obj.post_render_button_function)
					obj.post_render_button_function();
			glPopMatrix();
		}
		//Next draw its menu form IF it has one and is expanded
		if (obj.is_menu && (obj.expanded || !obj.nest) && (obj.draw_if_unfocused || obj.focused)) {
			glPushMatrix();
				glTranslatePoint(obj.menu_location);
				glScalePoint(obj.menu_scale);
				if (obj.pre_render_menu_function)
					obj.pre_render_menu_function();
				draw(obj.menu_body);
				for (unsigned int i = 0; i < obj.elements.size(); ++i) {
					draw(obj.elements[i]);
				}
				if (obj.post_render_menu_function)
					obj.post_render_menu_function();
			glPopMatrix();
		}
	}
}

#endif
