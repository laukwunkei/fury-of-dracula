////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// Map.c: an implementation of a Map type
// You can change this as much as you want!
// If you modify this, you should submit the new version.
//
// 2017-11-30	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <assert.h>

#include "Map.h"
#include "Places.h"
#include "Queue.h"
#define MAX_STEPS 1000

struct map {
	int nV; // number of vertices
	int nE; // number of edges
	ConnList connections[NUM_REAL_PLACES]; //connection represented in a list
};

static void addConnections(Map m);
static void addConnection(Map m, PlaceId v, PlaceId w, TransportType type);
static inline bool isSentinelEdge(Connection c);

static ConnList connListInsert(ConnList l, PlaceId v, TransportType type);
static bool connListContains(ConnList l, PlaceId v, TransportType type);
static void railFix(int numofSteps, int qL, ConnList nextvertix, Map map, int *nsteps, int *pred, int *visited, Queue nQueue);

////////////////////////////////////////////////////////////////////////

/** Creates a new map. */
// #vertices always same as NUM_REAL_PLACES
Map MapNew(void)
{
	Map m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Couldn't allocate Map!\n");
		exit(EXIT_FAILURE);
	}

	m->nV = NUM_REAL_PLACES;
	m->nE = 0;
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		m->connections[i] = NULL;
	}

	addConnections(m);
	return m;
}

/** Frees all memory allocated for the given map. */
void MapFree(Map m)
{
	assert (m != NULL);

	for (int i = 0; i < m->nV; i++) {
		ConnList curr = m->connections[i];
		while (curr != NULL) {
			ConnList next = curr->next;
			free(curr);
			curr = next;
		}
	}
	free(m);
}

////////////////////////////////////////////////////////////////////////

/** Prints a map to `stdout`. */
void MapShow(Map m)
{
	assert(m != NULL);

	printf("V = %d, E = %d\n", m->nV, m->nE);
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			printf("%s connects to %s by %s\n",
			       placeIdToName((PlaceId) i),
			       placeIdToName(curr->p),
			       transportTypeToString(curr->type)
			);
		}
	}
}

////////////////////////////////////////////////////////////////////////

/** Gets the number of places in the map. */
int MapNumPlaces(Map m)
{
	assert(m != NULL);
	return m->nV;
}

/** Gets the number of connections of a particular type. */
int MapNumConnections(Map m, TransportType type)
{
	assert(m != NULL);
	assert(transportTypeIsValid(type) || type == ANY);

	int nE = 0;
	for (int i = 0; i < m->nV; i++) {
		for (ConnList curr = m->connections[i]; curr != NULL; curr = curr->next) {
			if (curr->type == type || type == ANY) {
				nE++;
			}
		}
	}

	return nE;
}

////////////////////////////////////////////////////////////////////////

/// Add edges to Graph representing map of Europe
static void addConnections(Map m)
{
	assert(m != NULL);

	for (int i = 0; !isSentinelEdge(CONNECTIONS[i]); i++) {
		addConnection(m, CONNECTIONS[i].v, CONNECTIONS[i].w, CONNECTIONS[i].t);
	}
}

/// Add a new edge to the Map/Graph
static void addConnection(Map m, PlaceId start, PlaceId end, TransportType type)
{
	assert(m != NULL);
	assert(start != end);
	assert(placeIsReal(start));
	assert(placeIsReal(end));
	assert(transportTypeIsValid(type));

	// don't add edges twice
	if (connListContains(m->connections[start], end, type)) return;

	m->connections[start] = connListInsert(m->connections[start], end, type);
	m->connections[end]   = connListInsert(m->connections[end], start, type);
	m->nE++;
}

/// Is this the magic 'sentinel' edge?
static inline bool isSentinelEdge(Connection c)
{
	return c.v == -1 && c.w == -1 && c.t == ANY;
}

/// Insert a node into an adjacency list.
static ConnList connListInsert(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	ConnList new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate ConnNode");
		exit(EXIT_FAILURE);
	}
	
	new->p = p;
	new->type = type;
	new->next = l;
	return new;
}

/// Does this adjacency list contain a particular value?
static bool connListContains(ConnList l, PlaceId p, TransportType type)
{
	assert(placeIsReal(p));
	assert(transportTypeIsValid(type));

	for (ConnList curr = l; curr != NULL; curr = curr->next) {
		if (curr->p == p && curr->type == type) {
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////

ConnList MapGetConnections(Map m, PlaceId p)
{
	assert(placeIsReal(p));
	return m->connections[p]; 
}

////////////////////////////////////////////////////////////////////////
// My own function of map

// get the shortest path from source to every location in the map and output 
// as an array
int *MapGetShortestPathTo(PlaceId src, int roundNum, int playerNum) {

	Map g = MapNew();
	assert (g != NULL);
	
	//Initialize all the needed array at once//
	int *pred = malloc(sizeof(int) * NUM_REAL_PLACES);// We will return this array later
	int nsteps[NUM_REAL_PLACES];
	int visited[NUM_REAL_PLACES];
	for (int i = 0; i < NUM_REAL_PLACES; i++) {
		pred[i] = -1;
		nsteps[i] = MAX_STEPS;
		visited[i] = 0;
	}
	///////////////////////////////////////////
	
	nsteps[src] = 0;
	int rN; // get round number for each 
	Queue nQueue = newQueue();
	QueueJoin(nQueue, src);

	// interate through all of the location
	while (QueueIsEmpty(nQueue) == 0) {
		PlaceId qL = QueueLeave(nQueue);
		rN = roundNum + nsteps[qL]; // determine what is the current round
		visited[qL] = 1;
		// Calculate the number of steps hunters can move using rail
		int numofSteps = (playerNum + rN) % 4;
			
			//iterate through all possilble adjacent vertexs
			ConnList nextVertex = g->connections[qL];
			while (nextVertex != NULL ) {
				
				// If vertex has been vistied
				if (visited[nextVertex->p] == 1) {
					nextVertex = nextVertex->next;
					continue;
				}
				
				// if the connection type is rail
				if (nextVertex->type == RAIL) {
					// If hunter can't use rail in this round.
					if (numofSteps == 0) {
						nextVertex = nextVertex->next;
						continue;
					}
					railFix(numofSteps, qL, nextVertex, g, nsteps, pred, visited, nQueue);
					nextVertex = nextVertex->next;
					continue;
				}
				
				// if vertex has been visited, we need to do the relaxation 
				// Note that what I mean "visited" is that the data in the
				// concrete location has been filled by other iteration
				if (pred[nextVertex->p] != -1) {
					// find two path to a specific destination, needs to relax
					if (nsteps[qL] + 1 < nsteps[nextVertex->p]) {
						nsteps[nextVertex->p] = nsteps[qL] + 1;
						pred[nextVertex->p] = qL;
						nextVertex = nextVertex->next;
						continue;
					}
				}
				// if vertex is adjacent to current vertex and haven't been visited
				if (pred[nextVertex->p] == -1) {
					QueueJoin(nQueue, nextVertex->p);
					// If vertex haven't been railfix	
						pred[nextVertex->p] = qL;
						nsteps[nextVertex->p] = nsteps[qL] + 1;
				}
				nextVertex = nextVertex->next;
			}
		
		// move current vertex out of the set
	}
	
	return pred;
}

// This function implement the feature enabling hunters to hop cities through rail in one round
// Using recursion
static void railFix(int numofSteps, int qL, ConnList nextvertix, Map map, int *nsteps, int *pred, int *visited, Queue nQueue){
	
	// Base case: numofSteps is equap to 1
	if (numofSteps == 1) {
		if (nsteps[nextvertix->p] > nsteps[qL] + 1) {
			// If we this location we never achieved
			if (pred[nextvertix->p] == -1) {
				QueueJoin(nQueue, nextvertix->p);
			}
			nsteps[nextvertix->p] = nsteps[qL] + 1;
			pred[nextvertix->p] = qL;
		}

		return;
	}

	// interate through all the next vertix which is rail connected
	ConnList curr = map->connections[nextvertix->p];//point to the first adjacent node
	while (curr != NULL) {
		// If vertex has been visited
		if (visited[curr->p] == 1) {
			curr = curr->next;
			continue;
		}
		// Check if the vertex is a rail type
		if (curr->type == RAIL)
			railFix(numofSteps - 1 , qL, curr, map, nsteps, pred, visited, nQueue);
		curr = curr->next;
	}
	return;
}