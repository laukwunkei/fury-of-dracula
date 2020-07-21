////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here
void updateTrail(char *pastplays, PlaceId *trail);
void updateMove(char *pastplays, PlaceId *trail);
bool in_trail(PlaceId *trail, PlaceId place);
bool Has_hide(PlaceId *trail);
bool Has_DB(PlaceId *trail);
// TODO: ADD YOUR OWN STRUCTS HERE

struct draculaView {
	GameView game;
	PlaceId drac_trail[TRAIL_SIZE]; //real location
	PlaceId drac_move[TRAIL_SIZE]; //record of all moves
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}
	new->game = GvNew(pastPlays, messages);
	updateTrail(pastPlays, new->drac_trail);
	updateMove(pastPlays, new->drac_move);
	return new;
}

void DvFree(DraculaView dv)
{
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->game);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->game);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->game, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->game, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->game);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	*numTraps = 0;
	PlaceId *Traps_array = malloc(sizeof(int) * NUM_REAL_PLACES * 3);
	Traps_array = GvGetTrapLocations(dv->game, numTraps);
	return Traps_array;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	*numReturnedMoves = 0;
	PlaceId *all_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	all_place = GvGetReachableByType(dv->game, PLAYER_DRACULA, GvGetRound(dv->game), dv->drac_trail[0], true, false, true, numReturnedMoves);
	//Create a new array to mark whether it's a valid place
	PlaceId *correct_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	int valid_place = 0;
	for (int i = 0; i < *numReturnedMoves; i++){
		if (all_place[i] != ST_JOSEPH_AND_ST_MARY && !in_trail(dv->drac_trail, all_place[i])){
			correct_place[i] = 1;
			valid_place++;
		}
	}
	//read only correct_place[i] into output array
	PlaceId *output = malloc(sizeof(int) * NUM_REAL_PLACES);
	int index = 0;
	for (int i = 0; i < *numReturnedMoves; i++){
		if (correct_place[i]){
			output[index++] = all_place[i];
		}
	}
	*numReturnedMoves = valid_place;
	//add special movement
	int trail_length = 0;
	bool hide_status = Has_hide(dv->drac_trail); 
	bool DB_status = Has_DB(dv->drac_move); 
	for (trail_length = 0; trail_length < TRAIL_SIZE; trail_length++){
		if(dv->drac_trail[trail_length] == NOWHERE) break;
	}
	if(!hide_status){
		output[(*numReturnedMoves)++] = HIDE;
	} 
	if(!DB_status){
		for (int i = 0; i < GvGetRound(dv->game); i++){
			output[(*numReturnedMoves)++] = DOUBLE_BACK_1 + i;
		}
	}
	return output;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId *all_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	all_place = GvGetReachableByType(dv->game, PLAYER_DRACULA, GvGetRound(dv->game), dv->drac_trail[0], true, false, true, numReturnedLocs);
	//Create a new array to mark whether it's a valid place
	PlaceId *correct_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	int valid_place = 0;
	for (int i = 0; i < *numReturnedLocs; i++){
		if (all_place[i] != ST_JOSEPH_AND_ST_MARY && !in_trail(dv->drac_trail, all_place[i])){
			correct_place[i] = 1;
			valid_place++;
		}
	}
	//read only correct_place[i] into output array
	PlaceId *output = malloc(sizeof(int) * NUM_REAL_PLACES);
	int index = 0;
	for (int i = 0; i < *numReturnedLocs; i++){
		if (correct_place[i]){
			output[index++] = all_place[i];
		}
	}
	*numReturnedLocs = valid_place;
	return output;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	PlaceId *all_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	all_place = GvGetReachableByType(dv->game, PLAYER_DRACULA, GvGetRound(dv->game), dv->drac_trail[0], road, false, boat, numReturnedLocs);
	//Create a new array to mark whether it's a valid place
	PlaceId *correct_place = malloc(sizeof(int) * NUM_REAL_PLACES);
	int valid_place = 0;
	for (int i = 0; i < *numReturnedLocs; i++){
		if (all_place[i] != ST_JOSEPH_AND_ST_MARY && !in_trail(dv->drac_trail, all_place[i])){
			correct_place[i] = 1;
			valid_place++;
		}
	}
	//read only correct_place[i] into output array
	PlaceId *output = malloc(sizeof(int) * NUM_REAL_PLACES);
	int index = 0;
	for (int i = 0; i < *numReturnedLocs; i++){
		if (correct_place[i]){
			output[index++] = all_place[i];
		}
	}
	*numReturnedLocs = valid_place;
	return output;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}


/*Helper function*/

//Update all moves of trail size
void updateMove(char *pastplays, PlaceId *trail){
	for (int i = 0; i < TRAIL_SIZE; i++){
		trail[i] = NOWHERE;
	}
	char *string_end;
	char *index; 
	for (string_end = pastplays; *string_end != '\0'; string_end++); 
	for (index = &pastplays[32]; index < string_end - 1; index += 40){
		if (*index == '\0') break;
		PlaceId original_place;
		//normal case
		char abbrev[3] = {index[1], index[2], '\0'};
		original_place = placeAbbrevToId(abbrev);
		//update to the front of trail
		trail[0] = original_place;
		for (int i = TRAIL_SIZE-1; i > 0; i--){
			trail[i] = trail[i-1];
		}
	}
}

//Modified from update move
//update real location of dracular
void updateTrail(char *pastplays, PlaceId *trail){
	for (int i = 0; i < TRAIL_SIZE; i++){
		trail[i] = NOWHERE;
	}
	char *string_end;
	char *index; 
	for (string_end = pastplays; *string_end != '\0'; string_end++); 
	for (index = &pastplays[32]; index < string_end - 1; index += 40){
		if (*index == '\0') break;
		PlaceId original_place;
		//special case
		if      (index[1] == 'T' && index[2] == 'P') original_place = CASTLE_DRACULA;
        else if (index[1] == 'H' && index[2] == 'I') original_place = trail[0];
        else if (index[1] == 'D' && index[2] == '1') original_place = trail[1];
        else if (index[1] == 'D' && index[2] == '2') original_place = trail[2];
        else if (index[1] == 'D' && index[2] == '3') original_place = trail[3];
        else if (index[1] == 'D' && index[2] == '4') original_place = trail[4];
        else if (index[1] == 'D' && index[2] == '5') original_place = trail[5];
        else {
		//normal case
		char abbrev[3] = {index[1], index[2], '\0'};
		original_place = placeAbbrevToId(abbrev);
		}
		//update to the front of trail
		trail[0] = original_place;
		for (int i = TRAIL_SIZE-1; i > 0; i--){
			trail[i] = trail[i-1];
		}
	}
}

//This function determine whether a given place in on drac trail
bool in_trail(PlaceId *trail, PlaceId place){
    int i;
    for (i = 0; i < TRAIL_SIZE; i++)
        if (trail[i] == place) return true;
    return false;
}

bool Has_hide(PlaceId *trail){
	for(int i = 0; i < TRAIL_SIZE; i++){
		if(trail[i] == HIDE) return true;
	}
	return false;
}

bool Has_DB(PlaceId *trail){
	for(int i = 0; i < TRAIL_SIZE; i++){
		if(trail[i] >= DOUBLE_BACK_1 && DOUBLE_BACK_5 <= trail[i]) return true;
	}
	return false;
}

