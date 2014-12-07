/* Politics.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Politics.h"

#include "Format.h"
#include "GameData.h"
#include "Messages.h"
#include "PlayerInfo.h"
#include "Random.h"
#include "Ship.h"
#include "ShipEvent.h"

#include <algorithm>

using namespace std;



// Reset to the initial political state defined in the game data.
void Politics::Reset()
{
	reputationWith.clear();
	ResetProvocation();
	
	for(const auto &it : GameData::Governments())
		reputationWith[&it.second] = it.second.InitialPlayerReputation();
}



bool Politics::IsEnemy(const Government *first, const Government *second) const
{
	if(first == second)
		return false;
	
	// Just for simplicity, if one of the governments is the player, make sure
	// it is the first one.
	if(second == GameData::PlayerGovernment())
		swap(first, second);
	if(first == GameData::PlayerGovernment())
	{
		if(bribed.find(second) != bribed.end())
			return false;
		if(provoked.find(second) != provoked.end())
			return true;
		
		auto it = reputationWith.find(second);
		return (it != reputationWith.end() && it->second < 0.);
	}
	
	// Neither government is the player, so the question of enemies depends only
	// on the attitude matrix.
	return (first->AttitudeToward(second) < 0. || second->AttitudeToward(first) < 0.);
}



// Commit the given "offense" against the given government (which may not
// actually consider it to be an offense). This may result in temporary
// hostilities (if the even type is PROVOKE), or a permanent change to your
// reputation.
void Politics::Offend(const Government *gov, int eventType, int count)
{
	if(gov == GameData::PlayerGovernment())
		return;
	
	// If you bribe a government but then attack it, the effect of your bribe is
	// cancelled out.
	if(eventType & ShipEvent::PROVOKE)
	{
		auto it = bribed.find(gov);
		if(it != bribed.end())
			bribed.erase(it);
	}
	
	for(const auto &it : GameData::Governments())
	{
		const Government *other = &it.second;
		double weight = other->AttitudeToward(gov);
		
		// You can provoke a government even by attacking an empty ship, such as
		// a drone (count = 0, because count = crew).
		if(eventType & ShipEvent::PROVOKE)
		{
			if(weight > 0.)
				provoked.insert(other);
		}
		else if(count * weight)
		{
			double penalty = (count * weight) * other->PenaltyFor(eventType);
			if(eventType & ShipEvent::ATROCITY)
				reputationWith[other] = min(0., reputationWith[other]);
			
			reputationWith[other] -= penalty;
		}
	}
}



// Bribe the given government to be friendly to you for one day.
void Politics::Bribe(const Government *gov)
{
	bribed.insert(gov);
}



// Check if the given ship can land on the given planet.
bool Politics::CanLand(const Ship &ship, const Planet *planet) const
{
	if(!planet)
		return false;
	
	const Government *gov = ship.GetGovernment();
	const Government *systemGov = planet->GetSystem()->GetGovernment();
	if(gov != GameData::PlayerGovernment())
		return !IsEnemy(gov, systemGov);
	
	return CanLand(planet);
}



bool Politics::CanLand(const Planet *planet) const
{
	if(bribedPlanets.find(planet) != bribedPlanets.end())
		return true;
	
	return Reputation(planet->GetSystem()->GetGovernment()) >= planet->RequiredReputation();
}



bool Politics::CanUseServices(const Planet *planet) const
{
	auto it = bribedPlanets.find(planet);
	if(it != bribedPlanets.end())
		return it->second;
	
	return Reputation(planet->GetSystem()->GetGovernment()) >= planet->RequiredReputation();
}



// Bribe a planet to let the player's ships land there.
void Politics::BribePlanet(const Planet *planet, bool fullAccess)
{
	bribedPlanets[planet] = fullAccess;
}



// Check to see if the player has done anything they should be fined for.
string Politics::Fine(const PlayerInfo &player, const Government *gov, int scan, double security)
{
	// Do nothing if you have already been fined today, or if you evade
	// detection.
	auto it = fined.find(gov);
	if(it != fined.end() || Random::Real() > security || !gov->GetFineFraction())
		return "";
	
	string reason;
	int64_t maxFine = 0;
	for(const shared_ptr<Ship> &ship : player.Ships())
	{
		const vector<string> &licenses = ship->Licenses(gov);
		for(const string &name : licenses)
			if(player.GetCondition("license: " + name) <= 0)
			{
				const Outfit *outfit = GameData::Outfits().Get(name + " License");
				int64_t fine = outfit->Cost() / 10;
				if(!fine)
					fine = 100000;
				
				if((fine > maxFine && maxFine >= 0) || fine < 0)
				{
					maxFine = fine;
					reason = "operating a " + ship->ModelName() + " without a " + name + " License.";
				}
			}
		if(!scan || (scan & ShipEvent::SCAN_CARGO))
		{
			int64_t fine = ship->Cargo().IllegalCargoFine();
			if((fine > maxFine && maxFine >= 0) || fine < 0)
			{
				maxFine = fine;
				reason = "carrying illegal cargo.";
			}
		}
		if(!scan || (scan & ShipEvent::SCAN_OUTFITS))
		{
			for(const auto &it : ship->Outfits())
				if(it.second)
				{
					int64_t fine = it.first->Get("illegal");
					if((fine > maxFine && maxFine >= 0) || fine < 0)
					{
						maxFine = fine;
						reason = "having illegal outfits installed on your ship.";
					}
				}
		}
	}
	
	if(maxFine < 0)
	{
		Offend(gov, ShipEvent::ATROCITY);
		if(!scan)
			reason = "atrocity";
		else
			reason = "After scanning your ship, the " + gov->GetName()
				+ " captain hails you with a grim expression on his face. He says, \"You are guilty of "
				+ reason + " The penalty for your actions is death. Goodbye.\"";
	}
	else if(maxFine > 0)
	{
		// Scale the fine based on how lenient this government is.
		maxFine = maxFine * gov->GetFineFraction() + .5;
		reason = "The " + gov->GetName() + " fines you "
			+ Format::Number(maxFine) + " credits for " + reason;
		fined.insert(gov);
	}
	return reason;
}



// Disable fines for today (because the game was just loaded, so any fines
// were already checked for when you first landed).
void Politics::DisableFines()
{
	for(const auto &it : GameData::Governments())
		fined.insert(&it.second);
}



// Get or set your reputation with the given government.
double Politics::Reputation(const Government *gov) const
{
	auto it = reputationWith.find(gov);
	return (it == reputationWith.end() ? 0. : it->second);
}



void Politics::AddReputation(const Government *gov, double value)
{
	reputationWith[gov] += value;
}



void Politics::SetReputation(const Government *gov, double value)
{
	reputationWith[gov] = value;
}



// Reset any temporary provocation (typically because a day has passed).
void Politics::ResetProvocation()
{
	provoked.clear();
	bribed.clear();
	bribedPlanets.clear();
	fined.clear();
}
