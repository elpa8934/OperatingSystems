/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */

    // prediction for page needed when on P-X
    // 1 indicates most likely next page
    // 3 indicates a less likely next page
    static int needOnP0[20] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP1[20] = {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP2[20] = {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP3[20] = {3,0,0,0,1,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0};
    static int needOnP4[20] = {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP5[20] = {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP6[20] = {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP7[20] = {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
    static int needOnP8[20] = {3,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0};
    static int needOnP9[20] = {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0};
    static int needOnP10[20] = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0};
    static int needOnP11[20] = {3,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0};
    static int needOnP12[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0};
    static int needOnP13[20] = {3,0,0,0,0,0,0,0,0,3,0,0,0,0,1,0,0,0,0,0};
    static int needOnP14[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0};
    static int needOnP15[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0};
    static int needOnP16[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0};
    static int needOnP17[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0};
    static int needOnP18[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    static int needOnP19[20] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    static int *pageLikely[20] = {needOnP0, needOnP1, needOnP2, needOnP3, needOnP4,
	needOnP5, needOnP6, needOnP7, needOnP8, needOnP9, needOnP10, needOnP11,
	needOnP12, needOnP13, needOnP14, needOnP15, needOnP16, needOnP17, needOnP18,
	needOnP19};

    /* Local vars */
    int proc;
    int pc;
    int page;
    
    for(proc=0; proc < MAXPROCESSES; proc++){
	int pagesNeeded[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	pc = q[proc].pc; // program counter for process
	page = pc/PAGESIZE; // page the program counter needs 
	pagesNeeded[page] = 1; // mark the needed page
	// Loop through the likely needed pages after page needed by pc
	for (int i = 0; i < 20; i++) {
		if (pageLikely[page][i] > 0) { //Any page likely to be needed
		    pagesNeeded[i] = 1; //Mark as needed
		    if (pageLikely[page][i] == 1) { //most likely page
			// Look through the most likely page's next likely pages
			for (int j = 0; j < 20; j++) {
			    if (pageLikely[i][j] > 0) { //Likely to be needed
				pagesNeeded[j] = 1; //Mark as needed
			    }
			}
		    }
			
		}
	}
	for  (int i = 0; i < 20; i++) {
	    if (pagesNeeded[i]) {
		//Swap in pages that are likely to be needed
		pagein(proc, i);
	    } else {
		//Swap out pages not likely to be needed
		pageout(proc, i);
	    }
	}

    }			
} 
