////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
//Addition #include 

//My own declaration
#define MAX_TRAP 3
#define NO_TRAP 0

//This struct serves to keeping track of trap and vampire
struct CityInfo {
	int trap[MAX_TRAP];
	bool Vamp;
};

struct gameView {
	//Fundamental game status
	int gamescore;
	int characters_blood[5];// 这里将dracula和hunters的血量数组进行合并， chracters_blood[4] 代表dracula
							// 这样做的好处只会体现在等会儿如果用函数分工进行操作时， 可以只输入一个数组到函数中。
							// 因为我不知道分开显示血量的意义， 如果这个是重要的操作，请改回用两个数组分别表示。
	Round round;
	//enum type defined
	PlaceId loc_hist[NUM_PLAYERS][TRAIL_SIZE]; 
	Player curr_player;
	struct CityInfo cities[NUM_REAL_PLACES];
    //Still unsure what to do with message
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	// Initialize gamescore
	int i, j;
    new->gamescore= GAME_START_SCORE;
	// Initialize the blood of dracula and hunters respectively
	for (i = 0; i < NUM_PLAYERS - 1; i++)
        new->characters_blood[i] = GAME_START_HUNTER_LIFE_POINTS;

    new->characters_blood[5]= GAME_START_BLOOD_POINTS;
	// Initialize the location
    for (i = 0; i < NUM_PLAYERS; i++)
        for (j = 0; j < TRAIL_SIZE; j++)
            new->loc_hist[i][j] = NOWHERE;
	// Intialize the message of vampire and traps
    for (i=0; i < NUM_REAL_PLACES; i++ ) {
        for (j=0; j< MAX_TRAP; j++ )
            new->cities[i].trap[j] = NO_TRAP;
        new->cities[i].Vamp= false;
    }

	// Calculate how many rounds of games;
	int turnsofNew = numTurns(pastPlays);
	new->round = (int)(numTurns(pastPlays));
	
	// Analize the imported string
	char* abbr = strtok(pastPlays, " ");
	while(abbr != NULL) {
		// TODO: updates blood and location informations turn by turn;(Eric)
		// 这里用strok（）是想直接从pastPlays 提取出单个的string，然后根据这个string分析如何对血量和位置进行操作。
		// 当然， 若要对血量和location进行操作， 还要调用struct里面的loc_history
		// 我觉得这里可以用两个function分别对血量和location进行操作， 以实现分工合作， 具体function prototype我写了两个在下面

		abbr = strtok(pastPlays, NULL);
	}


	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
// input the game string(we can get it by use strtok) and location history
// return the numbers of blood increase or decrease
void bloodCal (char *Abb, PlaceId history [NUM_PLAYERS][TRAIL_SIZE], int characterBlood[5]);

void locateUpdate(char *Abb, PlaceId history [NUM_PLAYERS][TRAIL_SIZE]);

static int numTurns (char *pastPlays)
{
    //counts the spaces
    int count = 1;
    int i = 0;
    while (pastPlays[i] != 0) {
        if (pastPlays[i] == ' ') {
            count++;
        }
        i++;
    }
    return ((i == 0) ? 0 : count);
}
