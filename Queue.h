/***************************************************************************************
*    Title: Queue ADT 
*    Author: John Shepherd
* 	 From: Lab starter code from Comp2521 week07	
*    Availability: https://cgi.cse.unsw.edu.au/~cs2521/20T2/labs/week07/index.php
*
***************************************************************************************/

/* This is the jas's implementation for Queue ADT, We also slightly modify the source code to 
	fit this FOD program.
	
	Many thanks to jas!
*/

#ifndef QUEUE_H
#define QUEUE_H

#include "Item.h"

typedef struct QueueRep *Queue;

Queue newQueue (void);			// create new empty queue
void dropQueue (Queue);			// free memory used by queue
void showQueue (Queue);			// display as 3 > 5 > 4 > ...
void QueueJoin (Queue, Item);	// add item on queue
Item QueueLeave (Queue);		// remove item from queue
int QueueIsEmpty (Queue);		// check for no items

#endif
