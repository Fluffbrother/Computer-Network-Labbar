#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "node.h"
#include "sim_engine.h"
#include "util.h"

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	printdt(table, node);

	for (int i = 0; i < 4; i++) {
		if (i == node || is_neighbor(i, node) != 1) {
			continue;
		}
		tolayer2((struct rtpkt){
				.sourceid = node,
				.destid = i,
				.mincost =
						{
								table->costs[node][0],
								table->costs[node][1],
								table->costs[node][2],
								table->costs[node][3],
						},
		});
	}
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	static bool updated = false; 
	int send = pkt->sourceid;

	for(int i = 0; i < 4; i++){
		int new_cost = table->costs[send][node] + pkt->mincost[i];

		if(new_cost < table->costs[send][node]){
			table->costs[i][send] = new_cost;
			updated = true;
		}
	}
	// printdt(table, node);
	debug("UPDATE: %i\n", node);
}
