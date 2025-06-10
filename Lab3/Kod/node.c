// Daniel Brynne
// Erik Gunnar Reider

#include <stdbool.h>
#include <string.h>

#include "node.h"

#define SIZE 4
#define for_each_node(j) for (int j = 0; j < SIZE; i++)

static void cost_min(struct distance_table *table, int node) {
	// Find the shortest path
	int costmin[SIZE];
	for_each_node(i) {
		int min = INF;
		for_each_node(j) {
			int cost = table->costs[node][j] + table->costs[j][i];
			if (cost < min) {
				min = cost;
			}
		}
		costmin[i] = min;
	}

	// Send the new table to all neighbors
	for_each_node(i) {
		if (i == node || is_neighbor(i, node) != 1) {
			continue;
		}
		struct rtpkt packet;
		packet.sourceid = node;
		packet.destid = i;
		memcpy(packet.mincost, costmin, sizeof(costmin));

		tolayer2(packet);
	}
}

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	cost_min(table, node);
	printdt(table, node);
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	bool updated = false;
	int send = pkt->sourceid;

	// Update the min costs if there's a better/smaller number
	for_each_node(i) {
		if (table->costs[send][i] > pkt->mincost[i]) {
			updated = true;
			table->costs[send][i] = pkt->mincost[i];
		}
	}

	// Only send the new costs to the neighbors if the table was updated
	if (updated) {
		cost_min(table, node);
	}

	printdt(table, node);
}
