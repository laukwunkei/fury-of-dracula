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

	// Initialize the shortest path
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		new->hShortestP[i] = MapGetShortestPathTo(GvGetPlayerLocation(new->gv, i),GvGetRound(new->gv),i);
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
	PlaceId currLoc = NOWHERE;
	PlaceId currRound = HvGetRound(hv);
	revereseArray(moveHis, 0, numofReturned - 1);
	for (int i = 0; i < numofReturned; i++) {
		if (moveHis[i] != CITY_UNKNOWN && 
		moveHis[i] != SEA_UNKNOWN) {
			currLoc = moveHis[i];
			*round = currRound - i;
			break;
		}
	}
	// If function give information to notify we can free the string
	if (canFree == true)
		free(moveHis);
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
	#if 1
	int returnRound= -1;
	bool canFree = false;
	int *moveHis = GvGetMoveHistory(hv->gv, PLAYER_DRACULA, &returnRound, &canFree);
	revereseArray(moveHis, 0, returnRound - 1);
	
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
		trail[i] = moveHis[i];
	}
	
	return trail;
	#else
	return GvGetDraculaTrail(hv->gv);
	#endif
}



int *HvReturnMoveHis(HunterView hv, int *returnNumofRound, Player player) {

	bool canFree;	
	int *moveHis = GvGetMoveHistory(hv->gv, player, returnNumofRound, &canFree);
	revereseArray(moveHis, 0, *returnNumofRound - 1);
	return moveHis;
}
// // TODO
// void updateLocMovHv (HunterView hv, Player player, PlaceId newplace, PlaceId newmov) 
// {
// 	updateLocMov(hv -> gv, player, newplace, newmov);
// }
// // TODO
// void huntersResearch(HunterView hv)
// {
// 	Player all_players[NUM_PLAYERS - 1] = {PLAYER_LORD_GODALMING, PLAYER_DR_SEWARD, PLAYER_VAN_HELSING, PLAYER_MINA_HARKER};
// 	for(int i = 0; i < NUM_PLAYERS - 1; i++) {
// 		PlaceId cur_place = HvGetPlayerLocation(hv, all_players[i]);
// 		updateLocMovHv(hv, all_players[i], NOWHERE, cur_place); // stay in the same place
// 	}
// 	// whether to fetch the information of the vampire here?
// }

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
