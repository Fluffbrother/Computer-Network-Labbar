#include <stdio.h>

#include "sim_engine.h"
#include "node.h"

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table dt1;

void rtinit1() {
	for(int i = 0; i < 16; i++)
		dt1.costs[i % 4][i / 4] = 999;
	dt1.costs[1][0] = 1;
	dt1.costs[1][1] = 0;
	dt1.costs[1][2] = 1;
	dt1.costs[1][3] = 999;
	rtinit(&dt1, 1);
}

void rtupdate1(struct rtpkt *rcvdpkt) {
	rtupdate(&dt1, 1, rcvdpkt);
}

void linkhandler1(int linkid, int newcost) {
  /* DON'T CHANGE */
}

