/* Mission.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef MISSION_H_
#define MISSION_H_

#include "ConditionSet.h"
#include "Date.h"
#include "LocationFilter.h"
#include "MissionAction.h"
#include "NPC.h"

#include <list>
#include <map>
#include <memory>
#include <string>
#include <set>

class DataNode;
class DataWriter;
class Government;
class Planet;
class PlayerInfo;
class Ship;
class ShipEvent;
class System;
class UI;



// This class represents a mission that the player can take on, such as carrying
// cargo from one planet to another or defending or attacking a ship. Many of
// a mission's parameters can be randomized, either so that the same mission
// template can be reused many times, or just so the mission is not always
// exactly the same every time you replay the game.
class Mission {
public:
	// Load a mission, either from the game data or from a saved game.
	void Load(const DataNode &node);
	// Save a mission. It is safe to assume that any mission that is being saved
	// is already "instantiated," so only a subset of the data must be saved.
	void Save(DataWriter &out, const std::string &tag = "mission") const;
	
	// Basic mission information.
	const std::string &Name() const;
	const std::string &Description() const;
	// Check if this mission should be shown in your mission list. If not, the
	// player will not know this mission exists (which is sometimes useful).
	bool IsVisible() const;
	// Check if this mission has high priority. If any high-priority missions
	// are available, no others will be shown at landing or in the spaceport.
	// This is to be used for missions that are part of a series.
	bool HasPriority() const;
	
	// Find out where this mission is offered.
	enum Location {SPACEPORT, LANDING, JOB};
	bool IsAtLocation(Location location) const;
	
	// Information about what you are doing.
	const Planet *Destination() const;
	std::set<const System *> Waypoints() const;
	const std::string &Cargo() const;
	int CargoSize() const;
	int IllegalCargoFine() const;
	int Passengers() const;
	// The mission must be completed by this deadline (if there is a deadline).
	bool HasDeadline() const;
	const Date &Deadline() const;
	// If this mission's deadline was before the given date and it has not been
	// marked as failing already, mark it and return true.
	bool CheckDeadline(const Date &today);
	// Check if you have special clearance to land on your destination.
	bool HasClearance(const Planet *planet) const;
	// Get the string to be shown in the destination planet's hailing dialog. If
	// this is "auto", you don't have to hail them to get landing permission.
	const std::string &ClearanceMessage() const;
	// Check whether we have full clearance to land and use the planet's
	// services, or whether we are landing in secret ("infiltrating").
	bool HasFullClearance() const;
	
	// Check if it's possible to offer or complete this mission right now. The
	// check for whether you can offer a mission does not take available space
	// into account, so before actually offering a mission you should also check
	// if the player has enough space.
	bool CanOffer(const PlayerInfo &player) const;
	bool HasSpace(const PlayerInfo &player) const;
	bool CanComplete(const PlayerInfo &player) const;
	bool HasFailed(const PlayerInfo &player) const;
	
	// When the state of this mission changes, it may make changes to the player
	// information or show new UI panels. PlayerInfo::MissionCallback() will be
	// used as the callback for any UI panel that returns a value. If it is not
	// possible for this change to happen, this function returns false.
	enum Trigger {COMPLETE, OFFER, ACCEPT, DECLINE, FAIL, DEFER, VISIT};
	bool Do(Trigger trigger, PlayerInfo &player, UI *ui = nullptr) const;
	
	// Get a list of NPCs associated with this mission. Every time the player
	// takes off from a planet, they should be added to the active ships.
	const std::list<NPC> &NPCs() const;
	// If any event occurs between two ships, check to see if this mission cares
	// about it. This may affect the mission status or display a message.
	void Do(const ShipEvent &event, PlayerInfo &player, UI *ui);
	
	// "Instantiate" a mission by replacing randomly selected values and places
	// with a single choice, and then replacing any wildcard text as well.
	Mission Instantiate(const PlayerInfo &player) const;
	
	
private:
	std::string name;
	std::string displayName;
	std::string description;
	Location location = SPACEPORT;
	
	bool hasFailed = false;
	bool isVisible = true;
	bool hasPriority = false;
	bool hasDeadline = false;
	bool doDefaultDeadline = false;
	Date deadline;
	int daysToDeadline = 0;
	std::string clearance;
	LocationFilter clearanceFilter;
	bool hasFullClearance = true;
	
	int repeat = 1;
	std::string cargo;
	int cargoSize = 0;
	// Parameters for generating random cargo amounts:
	int cargoLimit = 0;
	double cargoProb = 0.;
	int illegalCargoFine = 0;
	int passengers = 0;
	// Parameters for generating random passenger amounts:
	int passengerLimit = 0;
	double passengerProb = 0.;
	
	ConditionSet toOffer;
	ConditionSet toComplete;
	ConditionSet toFail;
	
	const Planet *source = nullptr;
	LocationFilter sourceFilter;
	const Planet *destination = nullptr;
	LocationFilter destinationFilter;
	// Systems that must be visited:
	std::set<const System *> waypoints;
	std::map<const System *, MissionAction> onEnter;
	
	// NPCs:
	std::list<NPC> npcs;
	
	// Actions to perform:
	std::map<Trigger, MissionAction> actions;
};



#endif