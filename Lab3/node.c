#include "node.h"

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	printdt(table, node);
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	printdt(table, node);
}

