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
int *HvReturnTrail(HunterView hv) {
	#if 1
	int returnRound= -1;
	bool canFree = false;
	int *moveHis = GvGetMoveHistory(hv->gv, PLAYER_DRACULA, &returnRound, &canFree);
	// initialize the trail
	int *trail = malloc(sizeof(int) * TRAIL_SIZE);
	for (int i = 0; i < TRAIL_SIZE; i++) {
		trail[i] = NOWHERE;
	}

	for (int i = 0; i < TRAIL_SIZE; i++) {
		if (i < TRAIL_SIZE)
			trail[i] = moveHis[i];
		else
			break;
	}
	return trail;
	#else
	return GvGetDraculaTrail(hv->gv);
	#endif
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
