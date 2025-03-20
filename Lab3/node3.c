#include <stdio.h>

#include "sim_engine.h"
#include "node.h"

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table dt3;

void rtinit3() {
	for(int i = 0; i < 16; i++)
		dt3.costs[i % 4][i / 4] = 999;
	dt3.costs[3][0] = 7;
	dt3.costs[3][1] = 999;
	dt3.costs[3][2] = 2;
	dt3.costs[3][3] = 0;
	rtinit(&dt3, 3);
}

void rtupdate3(struct rtpkt *rcvdpkt) {
	rtupdate(&dt3, 3, rcvdpkt);
}

void linkhandler3(int linkid, int newcost) {
  /* DON'T CHANGE */
}

