/* Government.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Government.h"

#include "DataNode.h"
#include "GameData.h"
#include "ShipEvent.h"

#include <algorithm>

using namespace std;



// Default constructor.
Government::Government()
	: name("Uninhabited"), swizzle(0), color(1.), initialPlayerReputation(0.),
	bribe(0.), fine(1.), deathSentence(nullptr)
{
	// Default penalties:
	penaltyFor[ShipEvent::ASSIST] = -0.1;
	penaltyFor[ShipEvent::DISABLE] = 0.5;
	penaltyFor[ShipEvent::BOARD] = 0.3;
	penaltyFor[ShipEvent::CAPTURE] = 1.;
	penaltyFor[ShipEvent::DESTROY] = 1.;
	penaltyFor[ShipEvent::ATROCITY] = 10.;
}



// Load a government's definition from a file.
void Government::Load(const DataNode &node)
{
	if(node.Size() >= 2)
		name = node.Token(1);
	
	for(const DataNode &child : node)
	{
		if(child.Token(0) == "swizzle" && child.Size() >= 2)
			swizzle = child.Value(1);
		else if(child.Token(0) == "color" && child.Size() >= 4)
			color = Color(child.Value(1), child.Value(2), child.Value(3));
		else if(child.Token(0) == "player reputation" && child.Size() >= 2)
			initialPlayerReputation = child.Value(1);
		else if(child.Token(0) == "attitude toward")
		{
			for(const DataNode &grand : child)
				if(grand.Size() >= 2)
				{
					const Government *gov = GameData::Governments().Get(grand.Token(0));
					attitudeToward[gov] = grand.Value(1);
				}
		}
		else if(child.Token(0) == "penalty for")
		{
			for(const DataNode &grand : child)
				if(grand.Size() >= 2)
				{
					if(grand.Token(0) == "assist")
						penaltyFor[ShipEvent::ASSIST] = grand.Value(1);
					if(grand.Token(0) == "disable")
						penaltyFor[ShipEvent::DISABLE] = grand.Value(1);
					if(grand.Token(0) == "board")
						penaltyFor[ShipEvent::BOARD] = grand.Value(1);
					if(grand.Token(0) == "capture")
						penaltyFor[ShipEvent::CAPTURE] = grand.Value(1);
					if(grand.Token(0) == "destroy")
						penaltyFor[ShipEvent::DESTROY] = grand.Value(1);
					if(grand.Token(0) == "atrocity")
						penaltyFor[ShipEvent::ATROCITY] = grand.Value(1);
				}
		}
		else if(child.Token(0) == "bribe" && child.Size() >= 2)
			bribe = child.Value(1);
		else if(child.Token(0) == "fine" && child.Size() >= 2)
			fine = child.Value(1);
		else if(child.Token(0) == "death sentence" && child.Size() >= 2)
			deathSentence = GameData::Conversations().Get(child.Token(1));
	}
}



// Get the name of this government.
const string &Government::GetName() const
{
	return name;
}



// Get the color swizzle to use for ships of this government.
int Government::GetSwizzle() const
{
	return swizzle;
}



// Get the color to use for displaying this government on the map.
const Color &Government::GetColor() const
{
	return color;
}



// Get the government's initial disposition toward other governments or
// toward the player.
double Government::AttitudeToward(const Government *other) const
{
	if(other == this)
		return 1.;
	
	auto it = attitudeToward.find(other);
	return (it == attitudeToward.end() ? 0. : it->second);
}



double Government::InitialPlayerReputation() const
{
	return initialPlayerReputation;
}



// Get the amount that your reputation changes for the given offense.
double Government::PenaltyFor(int eventType) const
{
	double penalty = 0.;
	for(const auto &it : penaltyFor)
		if(eventType & it.first)
			penalty += it.second;
	return penalty;
}



// In order to successfully bribe this government you must pay them this
// fraction of your fleet's value. (Zero means they cannot be bribed.)
double Government::GetBribeFraction() const
{
	return bribe;
}



double Government::GetFineFraction() const
{
	return fine;
}



const Conversation *Government::DeathSentence() const
{
	return deathSentence;
}


	
// Check if, according to the politics stored by GameData, this government is
// an enemy of the given government right now.
bool Government::IsEnemy(const Government *other) const
{
	return GameData::GetPolitics().IsEnemy(this, other);
}
