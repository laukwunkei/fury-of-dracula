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

// 以下两个struct是为了将已经遍历过的path储存到hV里面防止重复遍历 //
/////////////// 这个是Hv.h里面老师提的需求 ///////////////////
typedef struct pathToDest {
	 PlaceId dest;
	 PlaceId* path;
	 PathToDest next;
} *PathToDest;

typedef struct hunterDest {
	PathToDest first;
	PathToDest tail;
} HunterDest;
///////////////////////////////////////////////////////////

struct hunterView {
	GameView gv;
	HunterDest listofDest[NUM_PLAYERS - 1]; //exclude dracula;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

static void freeHunterDest (HunterDest Hd[NUM_PLAYERS - 1]); 

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Initialize the struct of HvNew
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
	
	new->gv = GvNew(pastPlays,messages);

	for (int i = 0; i < (NUM_PLAYERS); i++) {
		new->listofDest[i].first = NULL;
		new->listofDest[i].tail = NULL;
	}

	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	freeHunterDest(hv->listofDest);
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*round = 0;
	return NOWHERE;
}
// 用bfs获得这个信息
PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	Map map = MapNew();

	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
// Free the array struct of HunterDest
static void freeHunterDest (HunterDest Hd[NUM_PLAYERS - 1]) {
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		while (Hd[i].first != NULL) {
			PathToDest tmp = Hd[i].first;
			Hd[i].first = Hd[i].first->next;
			free(tmp);
		}
	}
	free(Hd);
}
// TODO
