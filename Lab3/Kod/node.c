#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "node.h"
#include "sim_engine.h"
#include "util.h"

#define SIZE 4

void cost_min(struct distance_table *table, int node){
	int costmin[SIZE];
		for(int i = 0; i < SIZE; i++){
			int min  = INF;
			for(int j = 0; j < SIZE; j++){
				int cost;
				if(j == node){
					cost = table->costs[node][i];
				}
				else{
					cost = table->costs[node][j] + table->costs[j][i];
				}
				if(cost < min){
					min = cost;
				}
			}
			costmin[i] = min;
		}
		for(int i = 0; i < SIZE; i++){
			if (i == node || is_neighbor(i, node) != 1) {
				continue;
			}
			struct rtpkt packet;
			packet.sourceid = node;
			packet.destid = i;
			
			for(int j = 0; j < SIZE; j++){
				packet.mincost[j] = costmin[i];
			}
			tolayer2(packet);	
		}
}

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	printdt(table, node);
	cost_min(table, node);	
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	bool updated = false; 
	int send = pkt->sourceid;

	for(int i = 0; i < SIZE; i++){
		int new_cost = table->costs[send][node] + pkt->mincost[i];

		if(new_cost < table->costs[send][node]){
			table->costs[i][send] = new_cost;
			updated = true;
		}
	}
	if(updated){
		cost_min(table, node);
	}
	// printdt(table, node);
	debug("UPDATE: %i\n", node);
}

