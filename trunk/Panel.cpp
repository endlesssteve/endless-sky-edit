/* Panel.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Panel.h"

#include "Color.h"
#include "Command.h"
#include "FillShader.h"
#include "Screen.h"
#include "UI.h"

using namespace std;



// Constructor.
Panel::Panel()
	: ui(nullptr), isFullScreen(false), trapAllEvents(true)
{
}



// Make the destructor just in case any derived class needs it.
Panel::~Panel()
{
}



// Move the state of this panel forward one game step.
void Panel::Step()
{
}



// Draw this panel.
void Panel::Draw() const
{
}



// Return true if this is a full-screen panel, so there is no point in
// drawing any of the panels under it.
bool Panel::IsFullScreen()
{
	return isFullScreen;
}



// Return true if, when this panel is on the stack, no events should be
// passed to any panel under it. By default, all panels do this.
bool Panel::TrapAllEvents()
{
	return trapAllEvents;
}



// Only override the ones you need; the default action is to return false.
bool Panel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{
	return false;
}



bool Panel::Click(int x, int y)
{
	return false;
}



bool Panel::RClick(int x, int y)
{
	return false;
}



bool Panel::Hover(int x, int y)
{
	return false;
}



bool Panel::Drag(int dx, int dy)
{
	return false;
}



bool Panel::Scroll(int dx, int dy)
{
	return false;
}


	
void Panel::SetIsFullScreen(bool set)
{
	isFullScreen = set;
}



void Panel::SetTrapAllEvents(bool set)
{
	trapAllEvents = set;
}


	
// Dim the background of this panel.
void Panel::DrawBackdrop() const
{
	if(!GetUI()->IsTop(this))
		return;
	
	// Darken everything but the dialog.
	Color back(0., .7);
	FillShader::Fill(Point(), Point(Screen::Width(), Screen::Height()), back);
}



UI *Panel::GetUI() const
{
	return ui;
}



// This is not for overriding, but for calling KeyDown with only one or two
// arguments. In this form, the command is never set, so you can call this
// with a key representing a known keyboard shortcut without worrying that a
// user-defined command key will override it.
bool Panel::DoKey(SDL_Keycode key, Uint16 mod)
{
	return KeyDown(key, mod, Command());
}



void Panel::SetUI(UI *ui)
{
	this->ui = ui;
}