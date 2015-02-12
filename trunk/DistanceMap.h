/* DistanceMap.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef DISTANCE_MAP_H_
#define DISTANCE_MAP_H_

#include <map>

class Ship;
class System;
class PlayerInfo;



// This is a map of how many hyperspace jumps it takes to get to other systems
// from the given "center" system. Ships with a hyperdrive travel using the
// "links" between systems. Ships with jump drives can make use of those links,
// but can also travel to any of a system's "neighbors." A distance map can also
// be used to calculate the shortest route between two systems.
class DistanceMap {
public:
	// Find paths to the given system. If the given maximum count is above zero,
	// it is a limit on how many systems should be returned. If it is below zero
	// it specifies the maximum distance away that paths should be found.
	DistanceMap(const System *center, int maxCount = 0);
	// If a player is given, the map will only use hyperspace paths known to the
	// player; that is, one end of the path has been visited. Also, if the
	// player's flagship has a jump drive, the jumps will be make use of it.
	DistanceMap(const PlayerInfo &player);
	// Calculate the path for the given ship to get to the given system. The
	// ship will use a jump drive or hyperdrive depending on what it has.
	DistanceMap(const Ship &ship, const System *destination);
	
	// Find out if the given system is reachable.
	bool HasRoute(const System *system) const;
	// Find out how many jumps away the given system is.
	int Distance(const System *system) const;
	// If I am in the given system, going to the player's system, what system
	// should I jump to next?
	const System *Route(const System *system) const;
	
	// Access the distance map directly.
	const std::map<const System *, int> Distances() const;
	
	
private:
	void Init(int maxCount);
	void InitHyper(const PlayerInfo &player);
	void InitJump(const PlayerInfo &player);
	void InitHyper(const System *source);
	void InitJump(const System *source);
	
	
private:
	std::map<const System *, int> distance;
	std::map<const System *, const System *> route;
	bool hasJump = false;
};



#endif