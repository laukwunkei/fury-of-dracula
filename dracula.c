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
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Queue.h"
#include "Map.h"
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
static int findSafeRegion(DraculaView dv); 
static int find_region(PlaceId ID); 
static PlaceId MoveToRegion(DraculaView dv, region safe_region); 
static PlaceId MoveInRegion(DraculaView dv, region curr_region); 
static int distanceToClosestHunter(DraculaView dv);
static Queue findSafeMoves(DraculaView dv);
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
	|| VH_position == CASTLE_DRACULA || VH_position == CASTLE_DRACULA){
		DC_safe = false;
	}
	//Find starting point of the game
	region safe_region = findSafeRegion(dv);
	PlaceId curr_place = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	if (DvGetRound(dv) == 0){
		if(safe_region == NORTH_ENGLAND){
			registerBestPlay("LO", "Catch me at Big Ben");
		} else if(safe_region == SPAIN_PORTUGAL){
			registerBestPlay("BA", "Let's go FC Barcelona!");
		} else if(safe_region == SOUTHERN_EUROPE){
			registerBestPlay("FL", "Here's where enlightenment begins!");
		} else if(safe_region == Central_Europe){
			registerBestPlay("PA", "Quoi de neuf?");
		} else if(safe_region == Eastern_Europe && DC_safe){
			registerBestPlay("CD", "hehehehehe");
		} else if(safe_region == Eastern_Europe && !DC_safe){
			registerBestPlay("BE", "Catch me!");
		}
	}

	//Evaluate moves in between games 
	int blood_status = DvGetHealth(dv, PLAYER_DRACULA);
	PlaceId curr_region = find_region(curr_place);
	const char *next_move;
	//Moving towards CD if blood status is low
	if(DvGetHealth(dv, PLAYER_DRACULA) < 10){
		strcpy(next_move, placeIdToAbbrev(MoveToRegion(dv, Eastern_Europe)));
		registerBestPlay(next_move, "Catch me!");
	}
	//Always try to move towards region with least hunters if it not
	if(curr_region != safe_region){
		strcpy(next_move, placeIdToAbbrev(MoveToRegion(dv, safe_region)));
		registerBestPlay(next_move, "Keep moving!");
	}
	//Rotate around same region if given region is safe
	if(curr_region == safe_region){
		strcpy(next_move, placeIdToAbbrev(MoveInRegion(dv, curr_region)));
		registerBestPlay(next_move, "Catch me!");
	}
}

//Return region associate with particular place
static int find_region(PlaceId ID) {
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
static int find_SafeRegion(DraculaView dv){
	int N_Hunter[TOTAL_REGION] = {0};
	PlaceId hunter_position; 
	region hunter_region;
	for (int i = PLAYER_LORD_GODALMING; i < NUM_HUNTER; i++){
		hunter_position = DvGetPlayerLocation(dv, i);
		hunter_region = find_region(hunter_position);
		N_Hunter[hunter_region]++;
	}
	//Find the region with least hunter
	int smallest_index = 0;
	for (int i = 0; i < TOTAL_REGION; i++){
		if (N_Hunter[i] < N_Hunter[smallest_index]){
			smallest_index = i;
		}
	}
	return N_Hunter[smallest_index];
}

//Find the next move which is gives shortest path to given region
static PlaceId MoveToRegion(DraculaView dv, int safe_region){
	
	//set target in each region to move towards
	PlaceId target_place = UNKNOWN_PLACE;
	if (safe_region == NORTH_ENGLAND) target_place = LONDON;
	if (safe_region == SPAIN_PORTUGAL) target_place = BARCELONA;
	if (safe_region == SOUTHERN_EUROPE) target_place = FLORENCE;
	if (safe_region == Central_Europe) target_place = ZURICH;
	if (safe_region == Eastern_Europe) target_place = CASTLE_DRACULA;

	//Store possible connections
	int n_connections;
	PlaceId *connections = DvWhereCanIGo(dv, n_connections);
	PlaceId next_move = UNKNOWN_PLACE;

	//Return target if it's adjacent to target
	for (int i = 0; i < n_connections; i++){
		if (connections[i] == target_place){
			next_move = target_place;
			break;
		}
	}
	free(connections);
	//Finding the next moves that are closest to targe
	//or furthest from the minimal distance to nearest hunter
	if (next_move == UNKNOWN_PLACE){
		Queue possible_moves = findSafeMoves(dv);
		Map m = MapNew();
		PlaceId from;
		int min_step = 0;
		int dist_H = 0;
		while (QueueIsEmpty(possible_moves) != 0){
			from = QueueLeave(possible_moves);
			if(from > NUM_REAL_PLACES && QueueIsEmpty(possible_moves)){
				next_move = from; 
			} else {
				int temp_step = distance_sp(from, target_place, m);
				int temp_H_dist = minDistanceToClosestHunter(dv, m, from);
				//We want minimum step to target and furthest to closest hunter
				if(temp_step < min_step && temp_H_dist > dist_H){
					dist_H = temp_H_dist;
					min_step = temp_step;
					next_move = from;
				} else if(temp_step < min_step){
					min_step = temp_step;
					next_move = from;
				}
			}
		}
		dropQueue(possible_moves);
		MapFree(m);
	}
	return next_move;
}

//Find the next move which keeps drac within region
static PlaceId MoveInRegion(DraculaView dv, int curr_region){
	int next_location = -1;
	//Always heading towards CD when Drac arrive Eastern europe
	if (curr_region = Eastern_Europe && DvGetPlayerLocation(dv, PLAYER_DRACULA)!= CASTLE_DRACULA){
		next_location = MoveToRegion(dv, Eastern_Europe);
	} else {
		Queue possibleMoves = findSafeMoves(dv);
		while(!QueueIsEmpty(possibleMoves)){
			next_location = QueueLeave(possibleMoves);
			//make sure the next move is within the region
			if(curr_region == find_region(next_location)) break;
		}
		dropQueue(possibleMoves);
	}
	return next_location;
}

//Find safe moves that doesn't conflict to next possible hunters Locs
static Queue findSafeMoves(DraculaView dv){
	int n_moves;
	PlaceId *valid_moves = DvGetValidMoves(dv, &n_moves);
	Queue possible_moves = newQueue;

	//No real location moves
	if(valid_moves[0] > NUM_REAL_PLACES){
		if(n_moves == 1) 
			//Drac can only hide
			QueueJoin(possible_moves, valid_moves[0]);
		else 
			//Double back
			QueueJoin(possible_moves, valid_moves[n_moves-1]);
	}
	if (!QueueIsEmpty(possible_moves)){
		Queue q = QueueFromArray(n_moves, valid_moves);
		int *hunter_locs = PossibleHunterLocation(dv);
		while(!QueueIsEmpty(q)){
			int locs = QueueLeave(q);
			//Special case when no possible locs
			if (QueueIsEmpty(q) && QueueIsEmpty(possible_moves)){
				QueueJoin(possible_moves, locs);
				break;
			}
			//Skip with special moves
			if(locs >= 102 && locs <= 107){
				continue;
			}
			//Skip hunter possible locs
			if(hunter_locs[locs]) continue;
			QueueJoin(possible_moves, locs);
		}
	}
	return possible_moves;
}

//Return an array that contain both current and possible hunter location
static bool *PossibleHunterLocation(DraculaView dv){
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
		PlaceId *hunter_reachable = DvWhereCanTheyGo(dv, i, &n_reachable);
		for (int j = 0; j < n_reachable; j++){
			hunter_possible_locs[hunter_reachable[j]] = true;
		}
	}
}

//Return the closest distance to any hunter from a particular place
static int minDistanceToClosestHunter(DraculaView dv, Map m, PlaceId from){
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
static int ShortestPath_distance(PlaceId src, PlaceId dest, Map m){
	bool visited[NUM_REAL_PLACES] = {false};
    PlaceId pred[NUM_REAL_PLACES];
    for (int i = 0; i < NUM_REAL_PLACES; i++) pred[i] = NOWHERE;

    Queue q = newQueue();
	QueueJoin(q, src);

    while (queue_size(q) != 0) {
        int l = QueueLeave(q);
		ConnList curr = MapGetConnections(m, l);
        for (curr; curr != NULL; curr = curr->next) {

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
        count++;
    }
    return count;
}


