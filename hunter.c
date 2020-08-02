////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

// I define two modes of operation for hunters:
//
// Random move: When the hunter is in the trail or close to trail if he got the current location of dracula, 
// then move to dracula, otherwise he rondomly move
//
// On-purpose move: When the hunter is far from trail and dracula, it can move to any location near the trail as
// long as these location being revealed.

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include <time.h>
#include <stdlib.h>

<<<<<<< HEAD
typedef enum moveMode {
	RANDOM,
	ON_PURPOSE,
} MoveMode;


int randomMove(HunterView hv);
=======
void randomMove(HunterView hv);
>>>>>>> Justin2.0
int validLastLocation(HunterView hv);
int trailAnalized(int *trail);
int returnNext(int dest, int Player, HunterView hv);
bool farEnough(HunterView hv, int dest);
MoveMode moveMode(HunterView hv, int dest);


void decideHunterMove(HunterView hv)
{
	PlaceId locat = NOWHERE;
	int roundNum = 400;
	int currPlayer = HvGetPlayer(hv);
	
	
	
<<<<<<< HEAD
	// If hunter's blood is less than a perticular value, hunter rests.
	if (HvGetHealth(hv,HvGetPlayer(hv)) <= 4) {
		registerBestPlay(HvGetPlayerLocation(hv,HvGetPlayer(hv)), "Hunter rest");
		return;
	} 

	// Get the trail of dracula: We have to analyze the informations in the trail
	int *trail = HvReturnTrail(hv);
	
	// Trace vampire firstly in the early stage of game if any location of it revealed
	// If any hunter can get to the location of vampire before it become mature,
	// kill unmature vampire first 
	int vampLoc = HvGetVampireLocation(hv);
	if (vampLoc < 71 &&  vampLoc > -1 && HvGetScore(hv) > 300) {// We define "early stage of game" when game score
													// not lower than 300
		// Calculate how many turns it will mature
		int matureRound = -1;
		int distance;		
		for (int i = 0; i < 6; i++) {
			if (trail[i] == NOWHERE) 
				matureRound = 5 - i;
		}
		assert(matureRound != -1);

		// Eastimate whether current hunter can get there
		HvGetShortestPathTo(hv,HvGetPlayer(hv), vampLoc, &distance);
		if (distance <= matureRound) {
			int nextMove = returnNext(vampLoc, currPlayer, hv);
			if (farEnough(hv, nextMove)) 
				registerBestPlay(placeAbbrevToId(nextMove), "Have we nothing Toulouse?");
			else
				registerBestPlay(placeAbbrevToId(randomMove(hv)), "Have we nothing Toulouse?");
			return;
		}
	}
	

	// Check if there are useful informations available
	// If we can get informations about 
	locat= validLastLocation(hv);
	if(locat != NOWHERE) {

		// Traveral the trail 		
		for (int i = 0; i < TRAIL_SIZE; i++) {

			// If we found doubleback move
			if (trail[i] <= DOUBLE_BACK_5 && trail[i] >= DOUBLE_BACK_1) {

				// We find whether the location doubleback pointed has been revealed
				int backMove = trail[i] - DOUBLE_BACK_1 + 1;
				// Test if double back is valid
				assert(i - backMove < 0);
				// Previous location has been revealed
				if (trail[i - backMove] < 71 &&
				trail[i - backMove] > -1 ) {
					int nextMove = returnNext(trail[i - backMove], currPlayer, hv);
					
					// Eastimate which mode to use
					if (moveMode(hv, nextMove) == RANDOM)
						registerBestPlay(placeIdToAbbrev(randomMove(hv)), "Catch ya!");	
					else if (moveMode(hv, nextMove) == ON_PURPOSE)
						registerBestPlay(placeIdToAbbrev(nextMove), "Catch ya!");	

					return;
				} else {
					continue;
				}
			}
			
			// We found real location
			if (trail[i] < 71 && trail[i] > -1) {
				int nextMove = returnNext(trail[i], currPlayer, hv);
				// estimate which mode to use
				if (moveMode(hv, nextMove) == RANDOM)
					registerBestPlay(placeIdToAbbrev(randomMove(hv)), "Catch ya!");	
				else if (moveMode(hv, nextMove) == ON_PURPOSE)
					registerBestPlay(placeIdToAbbrev(nextMove), "Catch ya!");
				
				return;
			}
		}
	}

=======
	// Check if there are informations available
	// 这里我们需要看看现有的信息是否能反映出dracula大致的位置，这里的信息包括dracula的trail和
	// dracula最近移动的位置。
	// TODO：
	locat = HvGetLastKnownDraculaLocation(hv, &roundNum);
	draculaTrail = HvReturnTrail(hv);
>>>>>>> Justin2.0

	// If there are no informations available, hunter do researches
	// 这里如果我们发现现有的信息没办法反映出Dracula的具体位置，我们就可以让猎人研究以获得dracula
	// 的位置
	// TODO:
	if (!validLastLocation(hv)) {
		// research
		huntersResearch(hv);
	}

	registerBestPlay("TO", "Have we nothing Toulouse?");
}

// randomly choose a place from available location
void randomMove(HunterView hv) {
	Player all_players[NUM_PLAYERS - 1] = {PLAYER_LORD_GODALMING, PLAYER_DR_SEWARD, PLAYER_VAN_HELSING, PLAYER_MINA_HARKER};
	srand(time(NULL));
	for(int i = 0; i < NUM_PLAYERS - 1; i++) {
		int possible_locs;
		PlaceId *place_ids = HvWhereCanTheyGo(hv, all_players[i], &possible_locs);
		PlaceId random_place = place_ids[rand() % (possible_locs)];
		updateLocMovHv(hv, all_players[i], NOWHERE, random_place); // NOWHERE??? need double check
	}
}


// If we can get last location of dracula, we need to eastimate
// if this location is valid
// 这个就是让我们返回一个地点，我们需要在函数里面判断是否能得到dracula最后一次活动的位置
// 如果不能得到，返回NOWHERE。如果能得到，我们需要判断这个位置是否在六轮之内，（这个轮数可以后面修改）
// 如果不再六轮之内，我们说这个location是不够精确的，然后返回NOWHERE。如果在六轮之内，我们返回这个地点。 
int validLastLocation(HunterView hv) {
	Round cur_round = HvGetRound(hv);
	Round lastest_dracula_location_round;
	PlaceId place_id = HvGetLastKnownDraculaLocation(hv, &lastest_dracula_location_round);
	if ((place_id == NOWHERE) || (cur_round - lastest_dracula_location_round >= 6)) {
		return 0;
	} 
	return 1;
}

// 这个function里面我们分析dracula的trail（当然这个trail有可能什么都没有，毕竟给hunter的信息是有限的）
// 如果trail里面是有信息的，那在经过分析之后，我们返回最有用的那个location（具体怎么分析还没想好），如果trail里面
// 没有信息，我们返回NOWHERE.
<<<<<<< HEAD
int trailAnalized(int *trail) {
=======
int trailAnalized(HunterView hv) {
	// suggestion: 直接朝最近的place走
>>>>>>> Justin2.0
	return 0;
}

int returnNext(int dest, int Player, HunterView hv) {
	int *path;
	int pathLength;
	path = HvGetShortestPathTo(hv, Player, dest, &pathLength);
	return path[0];
}
bool farEnough(HunterView hv, int dest) {
	int pathLength; 
	HvGetShortestPathTo(hv, HvGetPlayer(hv), dest, &pathLength);
	if (pathLength > 6)
		return true;
	else
		return false;	

}


// Return move mode based on whether hunter is far from 
// the dracula and its trail.
MoveMode moveMode(HunterView hv, int dest) {
	// Get current location of dracula
	int *trail = HvReturnTrail(hv);
	int draculaCurrLoc = trail[0];
	
	if (farEnough(hv, dest) || draculaCurrLoc == dest) 
		return RANDOM;
	else 
		return ON_PURPOSE;
}