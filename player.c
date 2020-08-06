////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can  produce  either a hunter player or a Dracula player depending on
// the setting of the I_AM_DRACULA #define
//
// This  is  a  dummy  version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether  your  code  is
// likely to compile ...
//
// Note that this is used to drive both hunter and Dracula AIs. It first
// creates an appropriate view, and then invokes the relevant decideMove
// function,  which  should  use the registerBestPlay() function to send
// the move back.
//
// The real player.c applies a timeout, and will halt your  AI  after  a
// fixed  amount of time if it doesn 't finish first. The last move that
// your AI registers (using the registerBestPlay() function) will be the
// one used by the game engine. This version of player.c won't stop your
// decideMove function if it goes into an infinite loop. Sort  that  out
// before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.3	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "DraculaView.h"
#else
# include "hunter.h"
# include "HunterView.h"
#endif

// Moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `View',
// - functions `ViewNew', `decideMove', `ViewFree',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA

typedef DraculaView View;

# define ViewNew DvNew
# define decideMove decideDraculaMove
# define ViewFree DvFree

# define xPastPlays "GLO.... SLO.... HLO.... MLO.... DCD.V.. GSW.... SMN.... HMN.... MSW.... DGAT... GSW.... SSW.... HMN.... MSW.... DKLT... GSW.... SSW.... HMN.... MSW.... DBET... GSW.... SSW.... HSW.... MSW.... DBCT... GSW.... SSW.... HSW.... MSW.... DCNT... GSW.... SSW.... HSW.... MSW.... DBS..V. GSW.... SSW.... HSW.... MSW.... DVRT.M. GSW.... SSW.... HSW.... MSW.... DSOT.M. GSW.... SSW.... HSW.... MSW.... DBET.M. GSW.... SSW.... HSW.... MSW.... DKLT.M. GSW.... SSW.... HSW.... MSW.... DCDT.M. GSW.... SSW.... HSW.... MSW.... DGAT... GSW.... SSW.... HSW.... MSW.... DBC.VM. GSW.... SSW.... HSW.... MSW.... DCNT.M. GSW.... SSW.... HSW.... MSW.... DBS..M. GSW.... SSW.... HSW.... MSW.... DVRT.M. GSW.... SSW.... HSW.... MSW.... DSOT.M. GSW.... SSW.... HSW.... MSW.... DBET.M. GSW.... SSW.... HSW.... MSW.... DKLT.V. GSW.... SSW.... HSW.... MSW.... DCDT.M. GSW.... SSW.... HSW.... MSW.... DGAT... GSW.... SSW.... HSW.... MSW.... DBCT.M. GSW.... SSW.... HSW.... MSW.... DCNT.M. GSW.... SSW.... HSW.... MSW.... DBS..M. GSW.... SSW.... HSW.... MSW.... DVRT.M. GSW.... SSW.... HSW.... MSW.... DSO.VM. GSW.... SSW.... HSW.... MSW.... DBET.M. GSW.... SSW.... HSW.... MSW.... DKLT.M. GSW.... SSW.... HSW.... MSW.... DCDT.M. GSW.... SSW.... HSW.... MSW.... DGAT... GSW.... SSW.... HSW.... MSW...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GMA.... SPA.... HBR.... MSO.... DC?.V.. GCA.... SLE.... HFR.... MBC.... DC?T... GGR.... SNA.... HLI.... MBC.... DD1T... GMA.... SBO.... HCO.... MSZ.... DC?T... GSN.... SSR.... HST.... MBD.... DC?T... GLS.... SBA.... HGE.... MKL.... DLIT... GSR.... SBA.... HGE.... MKL.... DHIT.V. GMR.... STO.... HST.... MSZ.... DC?T.M. GZU.... SBO.... HNU.... MBC.... DS?..M. GMI.... SMR.... HCO.... MBC.... DC?T.M. GGE.... SGO.... HLITT.. MSZ.... DC?T.M. GST.... SVE.... HFR.... MJM.... DC?T... GCO.... SVE.... HZU.... MZA.... DD1T... GLI.... SBD.... HMI.... MVI.... DC?.VM. GFR.... SBC.... HMR.... MMU.... DHIT... GCO.... SBC.... HGE.... MMI.... DC?T.M. GLI.... SSO.... HFL.... MNP.... DC?T.M. GFR.... SVR.... HFL.... MTS.... DC?T.M. GZU.... SSZ.... HVE.... MRO.... DD2T.M. GMI.... SBD.... HVE.... MFL.... DC?T.V. GMR.... SKL.... HBD.... MFL.... DHIT.M. GGE.... SKL.... HBC.... MVE.... DS?..M. GFL.... SSZ.... HBC.... MVE.... DS?..M. GFL.... SBD.... HSO.... MGO.... DC?T.M. GVE.... SKL.... HVR.... MGE.... DD1T.M. GVE.... SKL.... HSZ.... MGE.... DC?T.M. GBD.... SBE.... HKL.... MST.... DC?.VM. GBC.... SJM.... HKL.... MNU.... DC?T... GBC.... SSJ.... HSZ.... MCO.... DHIT... GSO.... SSJ.... HBC.... MLI.... DC?T.M. GVR.... SSO.... HBC.... MFR.... DD1T.M. GSZ.... SVA.... HSO.... MZU.... DC?T.M. GBD.... SIO.... HVR.... MMI.... DC?T.V. GKL.... STS.... HSZ.... MMR.... DC?T.M. GKL.... SRO.... HBD.... MGE.... DHIT.M. GSZ.... SFL.... HKL.... MGE.... DC?T.M. GBD.... SGO.... HKL.... MFL.... DD1T.M. GVI.... SZU.... HSZ.... MVE.... DC?T.M. GZA.... SMI.... HBD.... MVE.... DC?T.M. GZA.... SMR.... HKL.... MBD.... DC?.VM. GVI.... SGE.... HKL.... MBC.... DHIT.M. GMU.... SFL.... HSZ.... MBC.... DC?T.M. GMI.... SFL.... HBD.... MSO.... DD1T.M. GNP.... SVE.... HKL.... MVR.... DC?T.M. GTS.... SVE.... HKL.... MSZ.... DC?T.M. GIO.... SMU.... HBD.... MZA.... DC?T.V. GBS.... SST.... HSZ.... MSJ.... DHIT.M. GVR.... SNU.... HKL.... MBE.... DC?T.M. GVR.... SFR.... HBD.... MBC.... DD1T.M. GBS.... SFR.... HBR.... MSO.... DC?T.M. GBS.... SBU.... HPR.... MSA.... DC?T.M. GVR.... SST.... HVI.... MBE.... DC?T.M. GVR.... SNU.... HSZ.... MBC.... DHI.VM. GBS.... SLI.... HVR.... MSO.... DC?T.M. GBS.... SZU.... HVR.... MSA.... DD1T.M. GCN.... SZU.... HBS.... MVR.... DC?T.M. GBC.... SGE.... HBS.... MCN.... DC?T.M. GGA.... SMI.... HVR.... MBC.... DC?T.M. GSZ.... SZU.... HVR.... MGA.... DHIT.V. GVE.... SZU.... HBS.... MSZ.... DC?T.M. GVE.... SGE.... HBS.... MVR.... DD1T.M. GGO.... SMI.... HCN.... MVR.... DC?T.M. GMR.... SZU.... HBC.... MBS.... DC?T.M. GMI.... SZU.... HGA.... MBS.... DC?T.M. GGO.... SGE.... HSZ.... MVR.... DHIT.M. GVE.... SMI.... HVI.... MVR.... DC?.VM. GMU.... SZU.... HPR.... MBS.... DD1T.M. GST.... SZU.... HBR.... MBS.... DC?T.M. GNU.... SGE.... HHA.... MVR.... DC?T.M. GFR.... SMI.... HNS.... MVR.... DC?T.M. GFR.... SZU.... HNS.... MBS.... DHIT.M. GBU.... SZU.... HAM.... MBS.... DC?T.V. GST.... SGE.... HAM.... MVR.... DD3T.M. GNU.... SMI.... HNS.... MVR.... DS?..M. GLI.... SZU.... HNS.... MBS.... DC?T.M. GBR.... SZU.... HAM.... MBS.... DC?T.M. GPR.... SGE.... HAM.... MVR.... DHIT.M. GVI.... SMI.... HNS.... MVR.... DC?T.M. GSZ.... SZU.... HNS.... MBS.... DD1.VM. GVR.... SZU.... HAM.... MBS.... DC?T... GSO.... SST.... HCO.... MIO.... DC?T.M. GBE.... SBU.... HBR.... MAS.... DC?T.M. GSA.... SAM.... HLI.... MVE.... DHIT.M. GIO.... SCO.... HLI.... MBD.... DC?T.M. GAS.... SBU.... HCO.... MBR.... DD3T.V. GVE.... SBO.... HAM.... MLI.... DS?..M. GBD.... SMR.... HNS.... MLI.... DC?T.M. GBR.... SGO.... HHA.... MCO.... DC?T.M. GLI.... STS.... HPR.... MAM.... DC?T.M. GLI.... SCG.... HNU...."
# define xMsgs { "", "", "" }


#endif

int main(void)
{
	char *pastPlays = xPastPlays;
	Message msgs[] = xMsgs;

	View state = ViewNew(pastPlays, msgs);
	decideMove(state);
	ViewFree(state);

	printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay(char *play, Message message)
{
	strncpy(latestPlay, play, MOVE_SIZE - 1);
	latestPlay[MOVE_SIZE - 1] = '\0';

	strncpy(latestMessage, message, MESSAGE_SIZE - 1);
	latestMessage[MESSAGE_SIZE - 1] = '\0';
}
