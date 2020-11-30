/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    
    /* VARIABLE USED FOR TESTING PREDICTIVE ALGORITHM
    static int lastPage = -1;
    */

    /* Local vars */
    int proctmp;
    int pagetmp;
    int proc;
    int pc;
    int page;
    int oldpage;
    int mostRecentTime;
    int toSwapOut;
    int pageTime;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }

    /* Select first active process */ 
    for(proc=0; proc<MAXPROCESSES; proc++) { 
	/* Is process active? */
	if(q[proc].active) {
	    /* Dedicate all work to first active process*/ 
	    pc = q[proc].pc; // program counter for process
	    page = pc/PAGESIZE; // page the program counter needs

	    /* USED FOR TESTING FOR PREDICTIVE ALGORITHM
	    if (proc == 1 && page != lastPage){
		printf("proc %d on page %d\n", proc, page);
		lastPage = page;
	    }
	    */

            // Update timestamps data structure - page is not recently used
            timestamps[proc][page] = tick;
	    /* Is page swaped-out? */
	    if(!q[proc].pages[page]) {
		/* Try to swap in, if swapping fails, swap out another page */
		if(!pagein(proc,page)) {
		    // Use LRU algorithm to find page to swap out
                    mostRecentTime = -1;
                    // Loop through all of the process's pages
		    // This is following local allocation
		    for(oldpage=0; oldpage < q[proc].npages; oldpage++) {
			pageTime = timestamps[proc][oldpage];
			// Find least recent page timestamp
	 		if (pageTime > mostRecentTime){
			    // Check that page is swapped in
			    if (q[proc].pages[oldpage]){
			        toSwapOut = oldpage;
				mostRecentTime = pageTime;
			    }
                        }
		    }
		    // Try to swap out that least revently used page
		    if(!pageout(proc, toSwapOut)){
			printf("Pageout failure. Inverstigate Error\n");
		    }
		}
	    }
	}
    /* advance time for next pageit iteration */
    tick++;
    }
} 
