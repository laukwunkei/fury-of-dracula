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

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
//Addition #include 
int FindTurn (char *pastPlays);
int is_substring (char *pastplay, char *sub, int index);
int originalMove (int index, char * pastPlays);
void updateLocMov (GameView gv, Player player, PlaceId newplace, PlaceId newmov);
void rvereseArray(int arr[], int start, int end);
//My own declaration
#define MAX_TRAP 3
#define HunterNum 4
#define MAX_MOVE 30
//This struct serves to keeping track of traps information of each city
struct CityInfo {
	bool trap[MAX_TRAP]; //Each city can have max 3 traps
};

struct gameView {
	//Fundamental game status
	int gamescore;
	int characters_blood[5]; //0 to 5 represented by playsequence  
	Round round;
	PlaceId VampLocation; //Where Immature Vamp Hide
	//enum type defined
	PlaceId trail_hist[NUM_PLAYERS][MAX_MOVE]; //This track of real location (show Dracular real location)
	PlaceId move_hist[NUM_PLAYERS][MAX_MOVE]; //This track of movement history (Dracular can hide/back)
	Player curr_player;
	struct CityInfo cities[NUM_REAL_PLACES]; //array of cityinfo 
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
	//initialise gameView variable 
	int curr_player = PLAYER_LORD_GODALMING;
	new->gamescore = GAME_START_SCORE;
	//initialise blood status
	new->characters_blood[4] = GAME_START_BLOOD_POINTS;
	for (int i = 0; i < 4; i++){
		new->characters_blood[i] = GAME_START_HUNTER_LIFE_POINTS;
	}
	//initialise trail history 
	for (int i = 0; i < NUM_PLAYERS; i++){
		for (int j = 0; j < MAX_MOVE; j++){
			new->trail_hist[i][j] = NOWHERE; 
		}
	}
	//initialise movement history
	for (int i = 0; i < NUM_PLAYERS; i++){
		for (int j = 0; j < MAX_MOVE; j++){
			new->move_hist[i][j] = NOWHERE; 
		}
	}
	//initialise city info 
	new->VampLocation = NOWHERE;
	for (int i = 0; i < NUM_REAL_PLACES; i++){
		for (int j = 0; j < MAX_TRAP; j++){
			new->cities[i].trap[j] = false;
		}
	}

	int total_turn = FindTurn(pastPlays);
	int curr_turn = 0;
	int index; 
	//reading until end of turn 
	while(curr_turn < total_turn){
		index = curr_turn * 8; 
		//Dracular term if mod5 is 0 
		if(is_substring(pastPlays, "D......", index)){
			//GameInfo update
			curr_player = PLAYER_DRACULA;
			new->gamescore -= SCORE_LOSS_DRACULA_TURN;
			char place_abbrev[3] = {pastPlays[index+1], pastPlays[index+2], '\0'};
			PlaceId new_place = placeAbbrevToId(place_abbrev);
			PlaceId actual_move = originalMove(index, pastPlays);
			updateLocMov(new, PLAYER_DRACULA, actual_move, new_place);

			if (placeIsSea(actual_move)) {
                new->characters_blood[PLAYER_DRACULA] -= LIFE_LOSS_SEA;
            }
            //drac at castle
            if (actual_move == TELEPORT || actual_move == CASTLE_DRACULA) {
                new->characters_blood[PLAYER_DRACULA] += LIFE_GAIN_CASTLE_DRACULA;
            }

			//Update with vampired and trap status
            // Immature vampired placed
            if (is_substring(pastPlays, "D...V..", index)) {
                new->VampLocation = new_place;
            }
            // Update trap status 
            if (is_substring(pastPlays, "D..T...", index)) {
                int i = 0;
                bool trap_updated = false;
                while (trap_updated != true && i < MAX_TRAP) {
                    if ( new->cities[actual_move].trap[i] == false ) {
                        new->cities[actual_move].trap[i] = true;
                        trap_updated = true;
                    }
                    i++;
                }
            }

            //vamp matures
            if (is_substring(pastPlays, "D....V.", index)) {
                new->gamescore -= SCORE_LOSS_VAMPIRE_MATURES;
				new->VampLocation = NOWHERE;
            }
		} else { //hunter term 
			if (is_substring(pastPlays, "G......", index)) curr_player = PLAYER_LORD_GODALMING;
            else if (is_substring(pastPlays, "S......", index)) curr_player = PLAYER_DR_SEWARD;
            else if (is_substring(pastPlays, "H......", index)) curr_player = PLAYER_VAN_HELSING;
            else if (is_substring(pastPlays, "M......", index)) curr_player = PLAYER_MINA_HARKER;

			//Updating new location for hunter
			char place_abbrev[3] = {pastPlays[index+1], pastPlays[index+2], '\0'};
			PlaceId new_place = placeAbbrevToId(place_abbrev);
			updateLocMov(new, curr_player, new_place, new_place);
			
			if (new_place == GvGetPlayerLocation(new, curr_player)){
				new->characters_blood[curr_player] += LIFE_GAIN_REST;
				if (new->characters_blood[curr_player] > 10){
					new->characters_blood[curr_player] = GAME_START_HUNTER_LIFE_POINTS;
				}
			} 
			if(new->characters_blood[curr_player] == 0 
				&& GvGetPlayerLocation(new, curr_player)){
					new->characters_blood[curr_player] = GAME_START_HUNTER_LIFE_POINTS;
				}

			int get_trapped = 0; //record how many trap encounter in one city
			//Encountering first traps in one city
			
            if (is_substring(pastPlays, "...T...", index)) {
                new->characters_blood[curr_player] -= LIFE_LOSS_TRAP_ENCOUNTER;
				get_trapped++;
            }
			//Encountering two traps in one city
			if (is_substring(pastPlays, "....T..", index)) {
                new->characters_blood[curr_player] -= LIFE_LOSS_TRAP_ENCOUNTER;
				get_trapped++;
            }
			//Encountering three traps in one city
			if (is_substring(pastPlays, ".....T.", index)) {
                new->characters_blood[curr_player] -= LIFE_LOSS_TRAP_ENCOUNTER;
				get_trapped++;
            }

			//update trap status
			while (get_trapped > 0){
				get_trapped--;
				new->cities[new_place].trap[get_trapped] = false;
			}

			//Encountering immature vampire
			if (is_substring(pastPlays, "...V...", index)||
			is_substring(pastPlays, "....V..", index)) {
                new->VampLocation = NOWHERE;
            }
			//Encountering dracular
			if (is_substring(pastPlays, "...D...", index)||
			is_substring(pastPlays, "....D..", index)||
			is_substring(pastPlays, ".....D.", index)||
			is_substring(pastPlays, "......D", index)) {
                new->characters_blood[curr_player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
                new->characters_blood[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
            }
			//Update game status if hunter dies from all encounterings
			if (new->characters_blood[curr_player] <= 0) {
                new->gamescore -= SCORE_LOSS_HUNTER_HOSPITAL;
                new->characters_blood[curr_player] = 0;
                new->trail_hist[curr_player][0] = ST_JOSEPH_AND_ST_MARY;
            } 
		}
		
		//update city info.
		for (int i = ADRIATIC_SEA; i < NUM_REAL_PLACES; i++){
			for (int j = 0; j < MAX_TRAP; j++){
				if (new->cities[i].trap[j] < curr_turn - ((TRAIL_SIZE-1)*NUM_PLAYERS)) {
                    new->cities[i].trap[j] = false;
                }
            }
		}
		curr_player = (curr_player + 1) % NUM_PLAYERS;
		curr_turn++;
	}
	new->round = curr_turn / NUM_PLAYERS;
	new->curr_player = curr_player;
	return new;
}

void GvFree(GameView gv)
{
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

//return current round number
Round GvGetRound(GameView gv)
{
	return gv->round;
}

//return current player 
Player GvGetPlayer(GameView gv)
{
	return gv->curr_player;
}

//return current score
int GvGetScore(GameView gv)
{
	return gv->gamescore;
}

int GvGetHealth(GameView gv, Player player)
{
	return gv->characters_blood[player];
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return gv->trail_hist[player][0];
}

PlaceId GvGetVampireLocation(GameView gv)
{
	return gv->VampLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	*numTraps = 0;
	PlaceId *Traps_array = malloc(sizeof(int) * NUM_REAL_PLACES * 3);
	//i denote as the PlaceId
	for(int i = ADRIATIC_SEA; i < NUM_REAL_PLACES; i++){
		for(int j = 0; j < MAX_TRAP; j++){
			if(gv->cities[i].trap[j] == true){
				Traps_array[*numTraps] = i;
				(*numTraps)++;
			}
		}
	}
	return Traps_array; //not sure about return type
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	*numReturnedMoves = 0;
	int total = 0;
	PlaceId *history_array = malloc(sizeof(int) * MAX_MOVE);
	for(int i = 0; gv->trail_hist[player][i]!= NOWHERE; i++){
		history_array[*numReturnedMoves] = gv->move_hist[player][i];
		(*numReturnedMoves)++;
		total++;
	}
	rvereseArray(history_array, 0, total-1);
	*canFree = false;
	return history_array;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	*numReturnedLocs = 0;
	int total = 0;
	PlaceId *history_array = malloc(sizeof(int) * MAX_MOVE);
	for(int i = 0; gv->trail_hist[player][i]!= NOWHERE; i++){
		history_array[*numReturnedLocs] = gv->trail_hist[player][i];
		(*numReturnedLocs)++;
		total++;
	}
	rvereseArray(history_array, 0, total-1);
	*canFree = false;
	return history_array;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	*numReturnedLocs = 0;
	int returned_locs = 0;
	PlaceId *last_move = malloc(sizeof(int) * MAX_MOVE);
	for(int i = 0; returned_locs < numLocs; i++){
		last_move[*numReturnedLocs] = gv->trail_hist[player][i];
		(*numReturnedLocs)++;
		returned_locs++;
	}
	*canFree = false;
	return last_move;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	Map game_map = MapNew();
	bool *reachable = malloc(sizeof(int) * MAX_MOVE);
	//Initialise and finding all reachable place
	for (int i = 0; i < NUM_REAL_PLACES; i++){
		reachable[i] = false;
	}
	reachable[from] = true;
	ConnList current = MapGetConnections(game_map, from);
	while (current != NULL){
		reachable[current->p] = true;
	}
	//Storing all reachable places 
	PlaceId *reachable_place = malloc(sizeof(int) * MAX_MOVE);
	*numReturnedLocs = 0;
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
        //don't allow dracula to go to the hospital
        if (reachable[i]) {
            reachable_place[*numReturnedLocs] = i;
			(*numReturnedLocs)++;
        }
    }
	/*
	ConnList curr = MapGetConnections(game_map, from);
	PlaceId *reachable_place = malloc(sizeof(int) * MAX_MOVE);
	*numReturnedLocs = 0;
	while (curr != NULL){
		reachable_place[*numReturnedLocs] = curr->p;
		(*numReturnedLocs)++;
		curr = curr->next;
	}*/
	return reachable_place;
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

//return total turn of gameplay
int FindTurn (char *pastPlays)
{
    //counts the spaces
    int count = 1;
	int i = 0;
	for (i = 0; pastPlays[i] != '\0'; i++){
		if (pastPlays[i] == ' ') {
            count++;
        }
	}
    return ((i == 0) ? 0 : count);
}

void updateLocMov (GameView gv, Player player, PlaceId newplace, PlaceId newmov){
	//update trail 
	for (int i = MAX_MOVE - 1; i > 0; i--){
		gv->trail_hist[player][i] = gv->trail_hist[player][i-1];
		gv->move_hist[player][i] = gv->move_hist[player][i-1];
	}
	gv->trail_hist[player][0] = newplace;
	gv->move_hist[player][0] = newmov;
}

int is_substring (char *pastplay, char *sub, int index)
{
    int i = 0; //position in sub string
    int match = true;

    while (pastplay[index + i] != '\0' && sub[i] != '\0') {
    	if (sub[i] != '.' || pastplay[index + i] == ' ' || pastplay[index + i] == '\0') {
        	if (sub[i] != pastplay[index + i]) {
            	match = false;
        	}
    	}
    	i++;
	}
    return match;
}

int originalMove (int index, char * pastPlays)
{
	char place_abbrev[3] = {pastPlays[index+1], pastPlays[index+2], '\0'};
    PlaceId curr = placeAbbrevToId(place_abbrev);
    //Dracular is cheecky. We need to traceback its real location
    while (HIDE <= curr && curr <= DOUBLE_BACK_5) {
        //Hide in previous city
        if (curr == HIDE) {
            index -= 40;
        } else {
            assert(curr <= DOUBLE_BACK_5 && curr >= DOUBLE_BACK_1);
            //If he doubled back, go back that many steps
            index -= (curr - DOUBLE_BACK_1 + 1) * 40;
        }
		char place_abbrev[3] = {pastPlays[index+1], pastPlays[index+2], '\0'};
        curr = placeAbbrevToId(place_abbrev);
    }
    return curr;
}

void rvereseArray(int arr[], int start, int end) 
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