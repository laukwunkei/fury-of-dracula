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
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE


///////////////////////////////////////////////////////////

struct hunterView {
	GameView gv;
	int *hShortestP[NUM_PLAYERS - 1]; // Hunters' shortest path
	// 这个二维数组用来储存四个hunter的最短路程
	// 所有初始化全部在huntervView初始化时进行
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
	// 这里用的nShortestP（）是我写在map.c的，具体实现可移步map.c
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		new->hShortestP[i] = MapGetShortestPathTo(GvGetPlayerLocation(new->gv, i),GvGetRound(new->gv),i);
	} 
	


	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		free(hv->hShortestP[i]);
	}
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetRound(hv->gv);
}

// What is the current player?
Player HvGetPlayer(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayer(hv->gv);
}


int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetScore(hv->gv);
}


int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(hv->gv, player);
}


PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(hv->gv, player);
}


PlaceId HvGetVampireLocation(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	for (int i = 0; i < numofReturned; i++) {
		if (moveHis[i] != CITY_UNKNOWN && 
		moveHis[i] != SEA_UNKNOWN && 
		moveHis[i] != CASTLE_DRACULA) {
			currLoc = moveHis[i];
			*round = i;
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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

// TODO
