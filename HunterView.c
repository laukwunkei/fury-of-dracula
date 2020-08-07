////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"

///////////////////////////////////////////////////////////
// Helper function

static void revereseArray(int arr[], int start, int end);
static bool isDoubleBack(PlaceId loc);

///////////////////////////////////////////////////////////
// Structs

struct hunterView {
	GameView gv;
	int *hShortestP[NUM_PLAYERS - 1]; // Hunters' shortest path
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor


HunterView HvNew(char *pastPlays, Message messages[])
{
	// Initialize the struct of HvNew
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
	new->gv = GvNew(pastPlays,messages);

	// If this is the first round of the game
	if (GvGetRound(new->gv) == 0) {
		for (int i = 0; i < NUM_PLAYERS - 1; i++) {
			new->hShortestP[i] = NULL;
		} 
	} else {
	// Initialize the shortest path
		for (int i = 0; i < NUM_PLAYERS - 1; i++) {
			new->hShortestP[i] = MapGetShortestPathTo(GvGetPlayerLocation(new->gv, i),GvGetRound(new->gv),i);
		} 
	}
	


	return new;
}

void HvFree(HunterView hv)
{
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		free(hv->hShortestP[i]);
	}
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

// What is the current player?
Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->gv);
}


int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->gv);
}


int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->gv, player);
}


PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->gv, player);
}


PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	int numofReturned = 0;
	bool canFree = false;
	PlaceId *moveHis = GvGetMoveHistory(hv->gv, PLAYER_DRACULA, &numofReturned, &canFree);
	PlaceId *tmp = malloc(sizeof(int) * numofReturned);
	for (int i = 0; i < numofReturned; i++) {
		tmp[i] = moveHis[i]; 
	}
	PlaceId currLoc = NOWHERE;
	PlaceId currRound = HvGetRound(hv);
	revereseArray(tmp, 0, numofReturned - 1);
	for (int i = 0; i < numofReturned; i++) {
		if (placeIsReal(tmp[i])) {
			currLoc = tmp[i];
			*round = currRound - i;
			break;
		}
		if (isDoubleBack(tmp[i]) ||
		tmp[i] == HIDE) {
			PlaceId realPlace = findFinalRealPlace(hv, i);
			if (placeIsReal(realPlace)) {
				currLoc = realPlace;
				*round = currRound - i;
				break;
			}
		}
		if (tmp[i] == TELEPORT) {
			currLoc = CASTLE_DRACULA;
			*round = currRound - i;
			break;
		}
	}
	// If function give information to notify we can free the string
	if (canFree == true)
		free(moveHis);
	free(tmp);
	return currLoc;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
		int src = GvGetPlayerLocation(hv->gv, hunter);
		// Count the number of destination.
		int curr = dest;
		int count = 1;
		while (hv->hShortestP[hunter][curr] != src) {
			count++;
			curr = hv->hShortestP[hunter][curr];
		}
		curr = dest;
		// allocate memory for the array
		PlaceId *path = malloc(sizeof(PlaceId) * count);
		// pass the shortest path into the array, then output
		path[count - 1] = dest;
		for (int i = 2; i <= count; i++) {
			path[count - i] = hv->hShortestP[hunter][curr];
			curr = hv->hShortestP[hunter][curr];
		}
	*pathLength = count;;
	return path;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	return GvGetReachable(hv->gv, GvGetPlayer(hv->gv), GvGetRound(hv->gv),GvGetPlayerLocation(hv->gv, GvGetPlayer(hv->gv)), numReturnedLocs);
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	return GvGetReachableByType(hv->gv, GvGetPlayer(hv->gv), GvGetRound(hv->gv), GvGetPlayerLocation(hv->gv, GvGetPlayer(hv->gv)), road, rail, boat, numReturnedLocs);
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	return GvGetReachable(hv->gv, player, GvGetRound(hv->gv),GvGetPlayerLocation(hv->gv, player), numReturnedLocs);
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	return GvGetReachableByType(hv->gv, player, GvGetRound(hv->gv), GvGetPlayerLocation(hv->gv, player), road, rail, boat, numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
int *HvReturnTrail(HunterView hv, int *trailLength) {
	
	int returnRound= -1;
	bool canFree = false;
	int *moveHis = GvGetMoveHistory(hv->gv, PLAYER_DRACULA, &returnRound, &canFree);
	// Initialize the tmp array
	int *tmp = malloc(sizeof(int) * returnRound);
	for (int i = 0; i < returnRound; i++) {
		tmp[i] = 0;
	}

	for (int i = 0; i < returnRound; i++) {
		tmp[i] = moveHis[i];
	}
	revereseArray(tmp, 0, returnRound - 1);
	
	// Whether the trail is bigger than 6
	if (returnRound < 6) 
		*trailLength = returnRound;
	else 
		*trailLength = TRAIL_SIZE;
	
	// initialize the trail
	int *trail = malloc(sizeof(int) * (*trailLength));
	for (int i = 0; i < *trailLength; i++) {
		trail[i] = NOWHERE;
	}

	for (int i = 0; i < *trailLength; i++) {
		trail[i] = tmp[i];
	}
	
	free(tmp);
	return trail;
}



int *HvReturnMoveHis(HunterView hv, int *returnNumofRound, Player player) {

	bool canFree;	
	int *moveHis = GvGetMoveHistory(hv->gv, player, returnNumofRound, &canFree);
	int *tmp = malloc(sizeof(int) * *returnNumofRound);
	for (int i = 0; i < *returnNumofRound; i++) {
		tmp[i] = moveHis[i];
	}
	revereseArray(tmp, 0, *returnNumofRound - 1);
	return tmp;
}


bool HvAnyOtherHunters(HunterView hv, PlaceId dest) {
	
	Player currPlayer = HvGetPlayer(hv);
	PlaceId currLocation[4];
	for (int i = 0; i < 4; i++) {
		currLocation[i] = HvGetPlayerLocation(hv, i);
	}
	// eatimate whether the location has other hunters
	for (int i = 0; i < 4; i++) {
		if (i == currPlayer)
			continue;
		if (currLocation[i] == dest)
			return true;
	}
	return false;
}

PlaceId findFinalRealPlace(HunterView hv, int numofRound) {
	
	int numofReturnedRound;
	PlaceId *moveHis = HvReturnMoveHis(hv, &numofReturnedRound, PLAYER_DRACULA);
	// Base case, find real place or Unknown place
	if (placeIsReal(moveHis[numofRound]) || 
	moveHis[numofRound] == TELEPORT ||
	moveHis[numofRound] == CITY_UNKNOWN ||
	moveHis[numofRound] == SEA_UNKNOWN) {
		if (moveHis[numofRound] == TELEPORT)
			return CASTLE_DRACULA;
		else
			return moveHis[numofRound];
	}

	// Not real place, eastimate which one
	if (isDoubleBack(moveHis[numofRound])) {
		int backMove = moveHis[numofRound] - DOUBLE_BACK_1 + 1;
		return findFinalRealPlace(hv, numofRound + backMove);
	} else if (moveHis[numofRound] == HIDE) 
		return findFinalRealPlace(hv, numofRound + 1);
	else {
		exit(EXIT_FAILURE);// We don't want this happened
	}

	
}

///////////////////////////////////////////////////////////////////
//Implementation of helper function

// Eastimate whether this is a double back move
static bool isDoubleBack(PlaceId loc) {
	if (loc <= DOUBLE_BACK_5 && 
	loc >= DOUBLE_BACK_1)
		return true;
	else
		return false;
}

// Written by my teammate Kwun Kei Lau in the GameView (Xu Bai)
// reverse the order of array
static void revereseArray(int arr[], int start, int end) 
{ 
    int temp; 
    while (start < end) 
    { 
        temp = arr[start];    
        arr[start] = arr[end]; 
        arr[end] = temp; 
        start++; 
        end--; 
    }    
}
