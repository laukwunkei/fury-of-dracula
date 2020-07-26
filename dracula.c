////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01 v1.0 Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01 v1.1 Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31 v2.0 Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10 v3.0 Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
/*Dracular strategy*/
/*My general strategy is based off dividing the map into regions
and find the number of hunters in each region to determine least
hunters area as safest region to move towards. Hence, it can divide
into 4 cases:
1. Find the furthest location to all hunters as starting point
2. When dracular land in a safe region: it should rotate in certain pattern ()
3. When it isn't safe, try to move in a new region (BFS)
4. Blood low as 10, try to move towards CD
*/

/*Defined regions*/
#define UNKOWN 0
#define NORTH_ENGLAND 1
#define SPAIN_PORTUGAL 2
#define SOUTHERN_EUROPE 3
#define Central_Europe 4
#define Eastern_Europe 5


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

void decideDraculaMove(DraculaView dv)
{
// TODO: Replace this with something better!
registerBestPlay("CD", "Mwahahassshaha");
}