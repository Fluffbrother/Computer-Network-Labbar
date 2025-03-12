#include <stdio.h>

#include "sim_engine.h"
#include "node.h"

extern int TRACE;
extern int YES;
extern int NO;

struct distance_table dt2;

void rtinit2() {
	for(int i = 0; i < 16; i++)
		dt2.costs[i % 4][i / 4] = 999;
	dt2.costs[2][0] = 3;
	dt2.costs[2][1] = 1;
	dt2.costs[2][2] = 0;
	dt2.costs[2][3] = 2;
	rtinit(&dt2, 2);
}

void rtupdate2(struct rtpkt *rcvdpkt) {
	rtupdate(&dt2, 2, rcvdpkt);
}

void linkhandler2(int linkid, int newcost) {
  /* DON'T CHANGE */
}

