////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Queue.h"
#include "Map.h"
#include <string.h>
#include "Places.h"
/*Simple Dracular strategy*/ 
/*My general strategy is based off dividing the map into regions 
and find the number of hunters in each region to determine least 
hunters area as safest region to move towards. Hence, it can divide 
into 4 cases:
1. Find the furthest location to all hunters as starting point 
2. When dracular land in a safe region: it should rotate in certain pattern
3. When it isn't safe, try to move in a new region (BFS)
4. Blood low as 10, try to move towards CD
*/

/*Helper function*/
int find_SafeRegion(DraculaView dv); 
int find_region(PlaceId ID); 
PlaceId MoveToRegion(DraculaView dv, int safe_region); 
PlaceId MoveInRegion(DraculaView dv, int curr_region); 
int ShortestPath_distance(PlaceId src, PlaceId dest, Map m);
int minDistanceToClosestHunter(DraculaView dv, Map m, PlaceId from);
bool *PossibleHunterLocation(DraculaView dv);
Queue findSafeMoves(DraculaView dv);
bool is_safe(DraculaView dv, int region);
bool moveIsLegal(DraculaView dv, PlaceId move);

/*Defined regions*/
typedef enum euro_regions {
	NORTH_ENGLAND,
	SPAIN_PORTUGAL,
	SOUTHERN_EUROPE,
	Central_Europe,
	Eastern_Europe
} region;

/*Defined constant*/
#define TOTAL_REGION 5
#define NUM_HUNTER 4


void decideDraculaMove(DraculaView dv)
{
	
	//storing all hunters current location first
	PlaceId LG_position = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	PlaceId DR_position = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
	PlaceId VH_position = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
	PlaceId MH_position = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
	//Determine whether Castle dracula is being occupied
	
	bool DC_safe = true;
	if(LG_position == CASTLE_DRACULA || DR_position == CASTLE_DRACULA
	|| MH_position == CASTLE_DRACULA || VH_position == CASTLE_DRACULA){
		DC_safe = false;
	}
	//Find starting point of the game
	int safe_region = find_SafeRegion(dv);
	PlaceId curr_place = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	//printf("curr round is %d\n", DvGetRound(dv));

	if (DvGetRound(dv) == 0){
		if(safe_region == Eastern_Europe && DC_safe){
			registerBestPlay("CD", "hehehehehe");
		} else if (safe_region == Eastern_Europe && !DC_safe){
			registerBestPlay("BE", "Catch me!");
		} else if(safe_region == NORTH_ENGLAND){
			registerBestPlay("LO", "Catch me at Big Ben");
		} else if(safe_region == SPAIN_PORTUGAL){
			registerBestPlay("BA", "Let's go FC Barcelona!");
		} else if(safe_region == SOUTHERN_EUROPE){
			registerBestPlay("FL", "Here's where enlightenment begins!");
		} else if(safe_region == Central_Europe){
			registerBestPlay("PA", "Quoi de neuf?");
		}
	} else {
		//Evaluate moves in between games 
		int blood_status = DvGetHealth(dv, PLAYER_DRACULA);
		int curr_region = find_region(curr_place);
		char *next_move = malloc(sizeof(char)*3);
		printf("drac is at %d region\n", curr_region);
		printf("safe region is %d\n", safe_region);
		//Moving towards CD if blood status is low
		if(blood_status < 18){
			strcpy(next_move, placeIdToAbbrev(MoveToRegion(dv, Eastern_Europe)));
			registerBestPlay(next_move, "Catch me!");
		}
		//Rotate around same region if given region is safe
		if(is_safe(dv, curr_region)){
			strcpy(next_move, placeIdToAbbrev(MoveInRegion(dv, curr_region)));
			registerBestPlay(next_move, "Catch me!");
		} else {
			strcpy(next_move, placeIdToAbbrev(MoveToRegion(dv, safe_region)));
			registerBestPlay(next_move, "Keep moving!");
		}
	}
}

//Return region associate with particular place
int find_region(PlaceId ID) {
    switch (ID) {
        case MANCHESTER:
        case GALWAY:
        case DUBLIN:
        case SWANSEA:
        case LIVERPOOL:
        case PLYMOUTH:
        case LONDON:
        case EDINBURGH:
        case AMSTERDAM:
        case HAMBURG:
        case LE_HAVRE:
        case ENGLISH_CHANNEL:
        case NORTH_SEA:
        case IRISH_SEA:
        case ATLANTIC_OCEAN:
            return NORTH_ENGLAND;
        case LISBON:
        case CADIZ:
        case MADRID:
        case GRANADA:
        case ALICANTE:
        case SARAGOSSA:
        case SANTANDER:
        case BARCELONA:
        case BAY_OF_BISCAY:
            return SPAIN_PORTUGAL;
        
        case TOULOUSE:
        case BORDEAUX:
        case MARSEILLES:
        case CAGLIARI:
        case CLERMONT_FERRAND:
		case NANTES:
		case GENOA:
		case FLORENCE:
		case ROME:
		case NAPLES:
		case BARI:
		case MEDITERRANEAN_SEA:
		case TYRRHENIAN_SEA:
            return SOUTHERN_EUROPE;
        case PARIS:
		case GENEVA:
		case MILAN:
		case VENICE:
		case BRUSSELS:
		case STRASBOURG:
		case ZURICH:
		case MUNICH:
		case NUREMBURG:
		case FRANKFURT:
		case COLOGNE:
		case LEIPZIG:
		case BERLIN:
		case PRAGUE:
			return Central_Europe;
		case VIENNA:
		case ZAGREB:
		case ST_JOSEPH_AND_ST_MARY:
		case BUDAPEST:
		case SZEGED:
		case SARAJEVO:
		case BELGRADE:
		case KLAUSENBURG:
		case CASTLE_DRACULA:
		case GALATZ:
		case CONSTANTA:
		case BLACK_SEA:
		case VARNA:
		case SOFIA:
		case SALONICA:
		case VALONA:
		case ATHENS:
		case IONIAN_SEA:
		case ADRIATIC_SEA:
		case BUCHAREST:
			return Eastern_Europe;
        default:
            break;
    }
    return 0;
}

//Finding the region with least amount of hunters
int find_SafeRegion(DraculaView dv){
	int N_Hunter[TOTAL_REGION] = {0};
	int min = 0;
	for (int i = 0; i < NUM_HUNTER; i++){
		N_Hunter[find_region(DvGetPlayerLocation(dv, i))]++;
	}
	for(int i = TOTAL_REGION - 1; i != 0; i--){
		if(N_Hunter[i] < N_Hunter[min]) min = i;
	}
	return min;
}

//Determine if the current region is safe
bool is_safe(DraculaView dv, int region){
	int count = 0;
	for (int i = 0; i < NUM_HUNTER; i++){
		if(find_region(DvGetPlayerLocation(dv, i)) == region){
			count++;
		}
	}
	if(count <= 1) return true;
	return false;
}

//Find the next move which is gives shortest path to given region
PlaceId MoveToRegion(DraculaView dv, int safe_region){
	//set target in each region to move towards
	PlaceId target_place = UNKNOWN_PLACE;
	if (safe_region == NORTH_ENGLAND) target_place = LONDON;
	if (safe_region == SPAIN_PORTUGAL) target_place = BARCELONA;
	if (safe_region == SOUTHERN_EUROPE) target_place = FLORENCE;
	if (safe_region == Central_Europe) target_place = ZURICH;
	if (safe_region == Eastern_Europe) target_place = CASTLE_DRACULA;
	//Store possible connections
	int n_connections;
	PlaceId *connections = DvWhereCanIGo(dv, &n_connections);
	//case when there's no real place, return the first non-real moves
	if (n_connections == 0){
		int n_moves;
		PlaceId *moves = DvGetValidMoves(dv, &n_moves);
		return moves[0];
	}

	PlaceId next_move = UNKNOWN_PLACE;
	//Return target if it's adjacent to target
	for (int i = 0; i < n_connections; i++){
		if (connections[i] == target_place){
			next_move = target_place;
			break;
		}
	}
	free(connections);
	//Finding the next moves that are closest to target
	//or furthest from the minimal distance to nearest hunter
	if (next_move == UNKNOWN_PLACE){
		Queue possible_moves = findSafeMoves(dv);
		//If no road route, go by double back or hide
		if(QueueSize(possible_moves)==0){
			int n_locs;
			PlaceId *moves = DvGetValidMoves(dv, &n_locs);
			return moves[0];
		}
		printf("safe move is\n");
		showQueue(possible_moves);
		Map m = MapNew();
		PlaceId from;
		int min_step = 100;
		while (QueueSize(possible_moves)!=0){
			from = QueueLeave(possible_moves);
			if(from > NUM_REAL_PLACES && QueueSize(possible_moves)==0){
				next_move = from; 
			} else {
				printf("from is %s\n", placeIdToAbbrev(from));
				int temp_step = ShortestPath_distance(from, target_place, m);
				printf("step is %d\n", temp_step);
				if(temp_step < min_step){
					min_step = temp_step;
					next_move = from;
				}
			}
			
		}
		dropQueue(possible_moves);
		MapFree(m);
	}
	printf("next is %s\n", placeIdToAbbrev(next_move));
	return next_move;
}

//Find the next move which keeps drac within region
PlaceId MoveInRegion(DraculaView dv, int curr_region){
	int next_location = -1;
	//Always heading towards CD when Drac arrive Eastern europe
	if (curr_region == Eastern_Europe && DvGetPlayerLocation(dv, PLAYER_DRACULA)!= CASTLE_DRACULA){
		next_location = MoveToRegion(dv, Eastern_Europe);
	} else {
		int n_locs = 0;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &n_locs);
		for(int i = 0; i < n_locs; i++){
			printf("i can go %d\n", locs[i]);
		}
		
		for(int i = 0; i < n_locs; i++){
			int index_region = find_region(locs[i]);
			if(index_region == curr_region){
				next_location = locs[i];
			}
		}
		if(next_location == -1){
			int n_moves = 0;
			PlaceId *moves = DvGetValidMoves(dv, &n_moves);
			next_location = moves[0];
		}
	}
	return next_location;
}

//Find safe moves that doesn't conflict to next possible hunters Locs
Queue findSafeMoves(DraculaView dv){
	int n_moves;
	PlaceId *valid_moves = DvWhereCanIGoByType(dv, true, false, &n_moves);
	Queue possible_moves = newQueue();
	//No real location moves
	if (QueueSize(possible_moves) == 0){
		Queue q = QueueFromArray(n_moves, valid_moves);
		showQueue(q);
		bool *hunter_locs = PossibleHunterLocation(dv);
		for(int i = 0;i < NUM_REAL_PLACES;i++){
			if(hunter_locs[i]) printf("possible hunter is at %d\n", i);
		}
		while(QueueSize(q)!=0){
			int locs = QueueLeave(q);
			//Special case when no possible locs
			if (QueueSize(q) == 0 && QueueSize(possible_moves) == 0){
				QueueJoin(possible_moves, locs);
				break;
			}
			//Skip hunter possible locs
			if(hunter_locs[locs]) continue;
			QueueJoin(possible_moves, locs);
		}
	}
	return possible_moves;
}
//Return an array that contain both current and possible hunter location
bool *PossibleHunterLocation(DraculaView dv){
	bool *hunter_possible_locs = malloc(sizeof(bool)*NUM_REAL_PLACES);
	//intialise the array as default false 
	for (int i = 0; i < NUM_REAL_PLACES; i++){
		hunter_possible_locs[i] = false;
	}
	
	//update current place for all hunters
	for (int i = 0; i < NUM_HUNTER; i++){
		hunter_possible_locs[DvGetPlayerLocation(dv, i)] = true;
	}
	
	//update next possible place for all hunters
	for (int i = 0; i < NUM_HUNTER; i++){
		int n_reachable;
		PlaceId *hunter_reachable = DvWhereCanTheyGoByType(dv, i, true, false, true, &n_reachable);
		for (int j = 0; j < n_reachable; j++){
			hunter_possible_locs[hunter_reachable[j]] = true;
		}
	}
	return hunter_possible_locs;
}
//Return the closest distance to any hunter from a particular place
int minDistanceToClosestHunter(DraculaView dv, Map m, PlaceId from){
	int distance[NUM_HUNTER] = {0};
	int min = MAX_REAL_PLACE;
	for (int i = 0; i < NUM_HUNTER; i++){
		distance[i] = ShortestPath_distance(from, DvGetPlayerLocation(dv, i), m);
		if(distance[i] < min){
			min = distance[i];
		}
	}
	return min;
}
//Returns the minimum no. of step to reach from src to dest
int ShortestPath_distance(PlaceId src, PlaceId dest, Map m){
	bool visited[NUM_REAL_PLACES] = {false};
    PlaceId pred[NUM_REAL_PLACES];
    for (int i = 0; i < NUM_REAL_PLACES; i++) pred[i] = NOWHERE;
    Queue q = newQueue();
	QueueJoin(q, src);
    while (!QueueIsEmpty(q)) {
        int l = QueueLeave(q);
		ConnList curr;
        for (curr = MapGetConnections(m, l); curr != NULL; curr = curr->next) {
			if (curr->type == RAIL) continue; //Drac can go on rail
            // Check if we've seen the location before
            if (!visited[curr->p]) {
                visited[curr->p] = true;
                pred[curr->p] = l;
                if (curr->p == dest) break;
                QueueJoin(q, curr->p);
            }
        }
    }
    int count = 0;
    PlaceId tmp = dest;
    while (pred[tmp] != src) {
        tmp = pred[tmp];
		printf("tmp is %s\n", placeIdToAbbrev(tmp));
        count++;
    }
    return count;
}
