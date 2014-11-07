/* InfoPanel.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef INFO_PANEL_H_
#define INFO_PANEL_H_

#include "Panel.h"

#include "ClickZone.h"
#include "ShipInfoDisplay.h"

#include <map>
#include <string>
#include <vector>

class Color;
class Outfit;
class PlayerInfo;



// This panel displays detailed information about the player's fleet and each of
// the ships in it. If the player is landed on a planet, this panel also allows
// them to reorder the ships in their fleet (including changing which one is the
// flagship) and to shift the weapons on any of their ships to different
// hardpoints.
class InfoPanel : public Panel {
public:
	InfoPanel(PlayerInfo &player);
	
	virtual void Draw() const override;
	
	
protected:
	// Only override the ones you need; the default action is to return false.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod) override;
	virtual bool Click(int x, int y) override;
	virtual bool Hover(int x, int y) override;
	
	
private:
	void UpdateInfo();
	void DrawInfo() const;
	void DrawShip() const;
	void DrawWeapon(int index, const Point &pos, const Point &hardpoint) const;
	
	
private:
	PlayerInfo &player;
	std::vector<std::shared_ptr<Ship>>::const_iterator shipIt;
	
	ShipInfoDisplay info;
	std::map<std::string, std::vector<const Outfit *>> outfits;
	
	mutable std::vector<ClickZone<int>> zones;
	int selected;
	int hover;
	Point hoverPoint;
	bool showShip;
};



#endif
