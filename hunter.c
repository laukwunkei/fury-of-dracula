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

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"


int randomMove(HunterView hv);
int validLastLocation(HunterView hv);
int trailAnalized(HunterView hv);

void decideHunterMove(HunterView hv)
{
	PlaceId locat = NOWHERE;
	int roundNum = 400;
	
	
	// If hunter's blood is less than a perticular value, hunter rests.
	if (HvGetHealth(hv,HvGetPlayer(hv)) <= 4) {
		registerBestPlay(HvGetPlayerLocation(hv,HvGetPlayer(hv)), "Hunter rest");
		return;
	} 
	// Check if there are useful informations available
	// useful informations refer to moves of dracula withing 6 rounds
	// even we can get the last move of dracula, we still need to analyse if there are 
	// any 
	locat= validLastLocation(hv);
	if(locat != NOWHERE) {
		#if 1
		// In this case, we trace unmatured vampire firstly
		// TODO:
		// In thsi case, we trace dracula directly
		// TODO:
		
		#else 
		trailAnalized(hv);
		#endif

	}


	// If there are no informations available, hunter do researches
	// 这里如果我们发现现有的信息没办法反映出Dracula的具体位置，我们就可以让猎人研究以获得dracula
	// 的位置
	// TODO:

	registerBestPlay("TO", "Have we nothing Toulouse?");
}

// randomly choose a place from available location
int randomMove(HunterView hv) {
	return 0;
}


// If we can get last location of dracula, we need to eastimate
// if this location is valid
// 这个就是让我们返回一个地点，我们需要在函数里面判断是否能得到dracula最后一次活动的位置
// 如果不能得到，返回NOWHERE。如果能得到，我们需要判断这个位置是否在六轮之内，（这个轮数可以后面修改）
// 如果不再六轮之内，我们说这个location是不够精确的，然后返回NOWHERE。如果在六轮之内，我们返回这个地点。 
int validLastLocation(HunterView hv) {
	return 0;
}

// 这个function里面我们分析dracula的trail（当然这个trail有可能什么都没有，毕竟给hunter的信息是有限的）
// 如果trail里面是有信息的，那在经过分析之后，我们返回最有用的那个location（具体怎么分析还没想好），如果trail里面
// 没有信息，我们返回NOWHERE.
int trailAnalized(HunterView hv) {
	return 0;
}