////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

// The strategy for hunters is to use research to get a rough direction of dracula using research 
// Once hunters getting closed to dracula, they randomly moves to find out the locations in the draculs's trail.
// In the first 6 rounds, hunters use random move to randomly find for dracula

// I define two modes of operation for hunters:
//
// Random move: When the hunter is in the trail or close to trail, if he got the current location of dracula, 
// then move to dracula, otherwise he randomly move
//
// On-purpose move: When the hunter is far from trail and dracula, it can move to any location near the trail as
// long as these location being revealed.

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

////////////////////////////////////
// Define two move mode
typedef enum moveMode {
	RANDOM,
	ON_PURPOSE,
} MoveMode;


////////////////////////////////////
// Helper functions
static int randomMove(HunterView hv, bool haveRail);
static int returnNext(int dest, int Player, HunterView hv);
static bool farEnough(HunterView hv, int dest);
static MoveMode moveMode(HunterView hv, int dest);
static bool isDoubleBack(PlaceId loc);


void decideHunterMove(HunterView hv)
{
	// When the game in the first round
    if(HvGetRound(hv) == 0){
    	if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) registerBestPlay("MA", "start here");
    	else if (HvGetPlayer(hv) == PLAYER_DR_SEWARD) registerBestPlay("PA", "start here");
    	else if (HvGetPlayer(hv) == PLAYER_VAN_HELSING) registerBestPlay("BR", "start here");
    	else if (HvGetPlayer(hv) == PLAYER_MINA_HARKER) registerBestPlay("SO", "start here");
    return;
    }

	int currPlayer = HvGetPlayer(hv);
	
	// If hunter's blood is less than a perticular value, hunter rests.
	if (HvGetHealth(hv,HvGetPlayer(hv)) <= 4) {
		int nextLoc = HvGetPlayerLocation(hv,HvGetPlayer(hv));
		registerBestPlay((char *)placeIdToAbbrev(nextLoc), "Hunter rest");
		return;
	} 

	// Get the trail of dracula: We have to analyze the informations in the trail
	int trailLength;
	int *trail = HvReturnTrail(hv, &trailLength);
	
	

	// Trace vampire if any location of it revealed
	// If any hunter can get to the location of vampire before it become mature,
	// kill unmature vampire first 
	int vampLoc = HvGetVampireLocation(hv);
	if (placeIsReal(vampLoc)) {
		// When vampire in the castle dracula
		if (vampLoc == CASTLE_DRACULA) {
			//DO NOTHING!
		} else {
			// Calculate how many turns it will mature
			int matureRound = -1;
			int distance;		
			for (int i = 0; i < trailLength; i++) {
				if (trail[i] == vampLoc) 
					matureRound = 5 - i;
			}

			// Eastimate whether current hunter can get there
			HvGetShortestPathTo(hv,HvGetPlayer(hv), vampLoc, &distance);
			if (distance <= matureRound) {
				int nextMove = returnNext(vampLoc, currPlayer, hv);
				registerBestPlay((char *)placeIdToAbbrev(nextMove), "Coming for unmature vampire");
				free(trail);
				return;
			}
		}
	}

		// Traveral dracula's trail
		for (int i = 0; i < trailLength; i++) {

			// If we found doubleback move or hide move or teleport
			if (isDoubleBack(trail[i]) || trail[i] == HIDE || trail[i] == TELEPORT) {

				PlaceId realLoc = findFinalRealPlace(hv, i);
				if (placeIsReal(realLoc)) {
					int nextMove = returnNext(realLoc, currPlayer, hv);
					
					// Eastimate which mode to use
					if (moveMode(hv, realLoc) == RANDOM)
						registerBestPlay((char *)placeIdToAbbrev(randomMove(hv, false)), "Playing around here");	
					else if (moveMode(hv, realLoc) == ON_PURPOSE)
						registerBestPlay((char *)placeIdToAbbrev(nextMove), "Come for dracula");
					free(trail);
					return;
				} else {
					continue;
				}
			}

			// We found real location
			if (placeIsReal(trail[i])) {
				int nextMove = returnNext(trail[i], currPlayer, hv);
				// estimate which mode to use
				if (moveMode(hv, trail[i]) == RANDOM)
					registerBestPlay((char *)placeIdToAbbrev(randomMove(hv, false)), "Playing around here");	
				else if (moveMode(hv, trail[i]) == ON_PURPOSE)
					registerBestPlay((char *)placeIdToAbbrev(nextMove), "Come for dracula");
				
				free(trail);
				return;
			}
		}
	
	
	// We can't find any useful information in the trail
	// If we are in the first 6 rounds, do random moves
	if (HvGetRound(hv) < 6) {
		registerBestPlay((char *)placeIdToAbbrev(randomMove(hv, true)), "Playing around here");
		return;
	}
	
	// Eastimate if hunters have researched withing 3 turns
	// If we have done research withing 3 rounds, we choose one move mode 
	// tring to find out the location of dracula
	int draculaLocRound;
	PlaceId draculaLastLoc= HvGetLastKnownDraculaLocation(hv, &draculaLocRound);
	if (HvGetRound(hv) - draculaLocRound - trailLength <= 3 &&
	draculaLastLoc != NOWHERE) {
		
		MoveMode mode = moveMode(hv, draculaLastLoc);
		if (mode == RANDOM)
			registerBestPlay((char *)placeIdToAbbrev(randomMove(hv, false)), "Playing around here");	
		else if(mode == ON_PURPOSE) {
			int nextMove = returnNext(draculaLastLoc, HvGetPlayer(hv), hv);
			registerBestPlay((char *)placeIdToAbbrev(nextMove), "Go find dracula's place");
		}

	} else { // We haven't done researches withing tree rounds, research!
		int nextMove = HvGetPlayerLocation(hv, HvGetPlayer(hv));
		registerBestPlay((char *)placeIdToAbbrev(nextMove), "Do some research");
	}
	free(trail);
	return;
}


// randomly choose a place from available location
// We will use the second variables to eastimate whether include rail
// In the meanwhile, we don't go to the same place we have been to
// in the last turn
static int randomMove(HunterView hv, bool haveRail) {
	// Generate a bunch of datas for random move
	srand(time(NULL));
	int numofLocs, numofReturnRound;// variables put into functions
	Player currPlayer = HvGetPlayer(hv);
	PlaceId *moveHis = HvReturnMoveHis(hv, &numofReturnRound, currPlayer);
	PlaceId *place_ids;
	
	// Eastimate whether consider the rail
	if (haveRail == false)
		place_ids = HvWhereCanTheyGoByType(hv, currPlayer, true, false, true, &numofLocs);
	else 
		place_ids = HvWhereCanTheyGoByType(hv, currPlayer, true, true, true, &numofLocs);
	int randomIndex = rand() % (numofLocs);
	
	// If current player only have one location in his move history
	if (numofReturnRound == 1)
		return place_ids[randomIndex];

	// We don't want hunter use random move to go the a place where he has already
	// visited in last round and other hunters in there
	for (int i = 0; i < numofLocs; i++) {
		// whether this random loacation has other hunters
		// if current hunter visited this random place in the last round
		if (HvAnyOtherHunters(hv, place_ids[randomIndex]) ||
		place_ids[randomIndex] == moveHis[1]) {
			if (randomIndex < numofLocs - 1)
				randomIndex++;
			else
				randomIndex = 0;
		} else {
			break;
		}

	}
	
	PlaceId randomPlace = place_ids[randomIndex];
	free(place_ids);
	free(moveHis);
	return randomPlace; 
}



// Return next location in the path from current location to destination
static int returnNext(int dest, int Player, HunterView hv) {
	PlaceId *path;
	PlaceId next;
	int pathLength;
	path = HvGetShortestPathTo(hv, Player, dest, &pathLength);
	next = path[0];
	free(path);
	return next;
}
 
// Whether current hunter is far enough from trail?
// We define the concept "far enough" to be as far as at least one move from 
// any moves revealed in draculs's trail, in another words, hunter should not be
// in the trail of dracula
static bool farEnough(HunterView hv, int dest) {
	int pathLength; 
	HvGetShortestPathTo(hv, HvGetPlayer(hv), dest, &pathLength);
	if (pathLength >= 3)
		return true;
	else
		return false;	
}


// Return move mode based on whether hunter is far from 
// the dracula and its trail.
static MoveMode moveMode(HunterView hv, int dest) {
	
	if(!placeIsReal(dest))
		return RANDOM;

	// Get current location of dracula
	PlaceId draculaCurrLoc = findFinalRealPlace(hv, 0);

	if (farEnough(hv, dest) || draculaCurrLoc == dest) 
		return ON_PURPOSE;
	else 
		return RANDOM;
}


static bool isDoubleBack(PlaceId loc) {
	if (loc <= DOUBLE_BACK_5 && 
	loc >= DOUBLE_BACK_1)
		return true;
	else
		return false;
}

