/* PlanetPanel.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef PLANET_PANEL_H_
#define PLANET_PANEL_H_

#include "Panel.h"

#include "Callback.h"
#include "GameData.h"
#include "Interface.h"
#include "Planet.h"
#include "PlayerInfo.h"
#include "System.h"
#include "WrappedText.h"

#include <memory>



// Dialog that pops up when you land on a planet. The shipyard and outfitter are
// shown in full-screen panels that pop up above this one, but the remaining views
// (trading, jobs, bank, port, and crew) are displayed within this dialog.
class PlanetPanel : public Panel {
public:
	PlanetPanel(const GameData &data, PlayerInfo &player, const Callback &callback);
	
	virtual void Draw() const;
	
	
protected:
	// Only override the ones you need; the default action is to return false.
	virtual bool KeyDown(SDLKey key, SDLMod mod);
	virtual bool Click(int x, int y);
	
	
private:
	bool FlightCheck();
	
	
private:
	const GameData &data;
	PlayerInfo &player;
	Callback callback;
	
	const Planet &planet;
	const System &system;
	const Interface &ui;
	
	std::shared_ptr<Panel> trading;
	std::shared_ptr<Panel> bank;
	std::shared_ptr<Panel> spaceport;
	Panel *selectedPanel;
	
	WrappedText text;
};



#endif
