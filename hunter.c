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

typedef enum moveMode {
	RANDOM,
	ON_PURPOSE,
} MoveMode;



int randomMove(HunterView hv);
int validLastLocation(HunterView hv);
int trailAnalized(int *trail);
int returnNext(int dest, int Player, HunterView hv);
bool farEnough(HunterView hv, int dest);
MoveMode moveMode(HunterView hv, int dest);


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

	PlaceId locat = NOWHERE;
	int currPlayer = HvGetPlayer(hv);
	
	// If hunter's blood is less than a perticular value, hunter rests.
	if (HvGetHealth(hv,HvGetPlayer(hv)) <= 4) {
		int nextLoc = HvGetPlayerLocation(hv,HvGetPlayer(hv));
		registerBestPlay((char *)placeIdToAbbrev(nextLoc), "Hunter rest");
		return;
	} 

	// Get the trail of dracula: We have to analyze the informations in the trail
	int *trail = HvReturnTrail(hv);
	
	// Trace vampire firstly in the early stage of game if any location of it revealed
	// If any hunter can get to the location of vampire before it become mature,
	// kill unmature vampire first 
	int vampLoc = HvGetVampireLocation(hv);
	if (vampLoc < 71 &&  vampLoc > -1 && HvGetScore(hv) > 300) {// We define "early stage of game" when game score
													// not lower than 300
		// Calculate how many turns it will mature
		int matureRound = -1;
		int distance;		
		for (int i = 0; i < 6; i++) {
			if (trail[i] == NOWHERE) 
				matureRound = 5 - i;
		}
		assert(matureRound != -1);

		// Eastimate whether current hunter can get there
		HvGetShortestPathTo(hv,HvGetPlayer(hv), vampLoc, &distance);
		if (distance <= matureRound) {
			int nextMove = returnNext(vampLoc, currPlayer, hv);
			if (farEnough(hv, nextMove)) 
				registerBestPlay((char *)placeIdToAbbrev(nextMove), "Have we nothing Toulouse?");
			else
				registerBestPlay((char *)placeIdToAbbrev(randomMove(hv)), "Have we nothing Toulouse?");
			return;
		}
	}
	

	// Check if there are useful informations available
	// If we can get informations about 
	locat= validLastLocation(hv);
	if(locat != NOWHERE) {

		// Traveral the move history
		for (int i = 0; i < TRAIL_SIZE; i++) {

			// If we found doubleback move
			if (trail[i] <= DOUBLE_BACK_5 && trail[i] >= DOUBLE_BACK_1) {

				// We find whether the location doubleback pointed has been revealed
				int backMove = trail[i] - DOUBLE_BACK_1 + 1;

				assert(i - backMove < 0);
				// Previous location has been revealed
				if (trail[i - backMove] < 71 &&
				trail[i - backMove] > -1 ) {
					int nextMove = returnNext(trail[i - backMove], currPlayer, hv);
					
					// Eastimate which mode to use
					if (moveMode(hv, trail[i - backMove]) == RANDOM)
						registerBestPlay((char *)placeIdToAbbrev(randomMove(hv)), "Catch ya!");	
					else if (moveMode(hv, trail[i - backMove]) == ON_PURPOSE)
						registerBestPlay((char *)placeIdToAbbrev(nextMove), "Catch ya!");	

					return;
				} else {
					continue;
				}
			}
			
			// We found real location
			if (trail[i] < 71 && trail[i] > -1) {
				int nextMove = returnNext(trail[i], currPlayer, hv);
				// estimate which mode to use
				if (moveMode(hv, trail[i]) == RANDOM)
					registerBestPlay((char *)placeIdToAbbrev(randomMove(hv)), "Catch ya!");	
				else if (moveMode(hv, trail[i]) == ON_PURPOSE)
					registerBestPlay((char *)placeIdToAbbrev(nextMove), "Catch ya!");
				
				return;
			}
		}
	}
	
	// We can't find any useful information in the trail
	
	// If we are in the first 6 rounds, do random moves
	if (HvGetRound(hv) < 6) {
		registerBestPlay((char *)placeIdToAbbrev(randomMove(hv)), "Playing around here");
		return;
	}
	
	// Eastimate if hunters have researched withing 3 turns
	// If we have done research withing 3 rounds, we choose one move mode 
	// tring to find out the location of dracula
	int draculaLocRound;
	PlaceId draculaLastLoc= HvGetLastKnownDraculaLocation(hv, &draculaLocRound);
	if (HvGetRound(hv) - draculaLocRound - TRAIL_SIZE <= 3) {
		
		MoveMode mode = moveMode(hv, draculaLastLoc);
		if (mode == RANDOM)
			registerBestPlay((char *)placeIdToAbbrev(randomMove(hv)), "Playing around here");	
		else if(mode == ON_PURPOSE)
			registerBestPlay((char *)placeIdToAbbrev(draculaLocRound), "Go find dracula's place");


	} else { // We haven't done researches withing tree rounds, research!
		int nextMove = HvGetPlayerLocation(hv, HvGetPlayer(hv));
		registerBestPlay((char *)placeIdToAbbrev(nextMove), "Do some research");
	}
	return;
}


// randomly choose a place from available location
// In the meanwhile, we don't go to the same place we have been to
// in the last turn
int randomMove(HunterView hv) {
	srand(time(NULL));
	int numofLocs;
	PlaceId *place_ids = HvWhereCanTheyGo(hv, HvGetPlayer(hv) ,&numofLocs);
	PlaceId random_place = place_ids[rand() % (numofLocs)];
	return random_place;
}


// If we can get last location of dracula, we need to eastimate
// if this location is valid
int validLastLocation(HunterView hv) {
	Round cur_round = HvGetRound(hv);
	Round lastest_dracula_location_round;
	PlaceId place_id = HvGetLastKnownDraculaLocation(hv, &lastest_dracula_location_round);
	if ((place_id == NOWHERE) || (cur_round - lastest_dracula_location_round >= 6)) {
		return NOWHERE;
	} 
	return place_id;
}

// Return next location in the path from current location to destination
int returnNext(int dest, int Player, HunterView hv) {
	int *path;
	int pathLength;
	path = HvGetShortestPathTo(hv, Player, dest, &pathLength);
	return path[0];
}
 
// Whether current hunter is far enough from trail?
// We define the concept "far enough" to be as far as at least one move from 
// any moves revealed in draculs's trail, in another words, hunter should not be
// in the trail of dracula
bool farEnough(HunterView hv, int dest) {
	int pathLength; 
	HvGetShortestPathTo(hv, HvGetPlayer(hv), dest, &pathLength);
	if (pathLength > 1)
		return true;
	else
		return false;	
}


// Return move mode based on whether hunter is far from 
// the dracula and its trail.
MoveMode moveMode(HunterView hv, int dest) {
	// Get current location of dracula
	int *trail = HvReturnTrail(hv);
	int draculaCurrLoc = trail[0];
	
	if (farEnough(hv, dest) || draculaCurrLoc == dest) 
		return ON_PURPOSE;
	else 
		return RANDOM;
}