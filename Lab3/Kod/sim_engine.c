#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sim_engine.h"

#define LINKCHANGES 0

int TRACE = 1;
int YES = 1;
int NO = 0;

void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid,
                 int *mincosts) {
  int i;
  initrtpkt->sourceid = srcid;
  initrtpkt->destid = destid;
  for (i = 0; i < 4; i++)
    initrtpkt->mincost[i] = mincosts[i];
}

struct event {
  float evtime; /* Event time */
  int evtype;   /* Event type code */
  int eventity; /* Entity where event occurs */
  struct rtpkt
      *rtpktptr; /* Pointer to packet (if any) associated with this event */
  struct event *prev;
  struct event *next;
};

struct event *evlist = NULL; /* The event list */

/* Possible events */
#define FROM_LAYER2 2
#define LINK_CHANGE 10

float clocktime = 0.000;

void init();
void printevlist();
float jimsrand();
void insertevent(struct event *p);

int main() {
  struct event *eventptr;

  init();

  while (1) {

    eventptr = evlist; /* Get next event to simulate */
    if (eventptr == NULL)
      goto terminate;
    evlist = evlist->next; /* Remove this event from event list */
    if (evlist != NULL)
      evlist->prev = NULL;
    if (TRACE > 1) {
      printf("MAIN: Receive event, t=%.3f, at %d", eventptr->evtime,
             eventptr->eventity);
      if (eventptr->evtype == FROM_LAYER2) {
        printf(" Source:%2d,", eventptr->rtpktptr->sourceid);
        printf(" Destinaton:%2d,", eventptr->rtpktptr->destid);
        printf(" Contents: %3d %3d %3d %3d\n", eventptr->rtpktptr->mincost[0],
               eventptr->rtpktptr->mincost[1], eventptr->rtpktptr->mincost[2],
               eventptr->rtpktptr->mincost[3]);
      }
    }
    clocktime = eventptr->evtime; /* Update time to next event time */
    if (eventptr->evtype == FROM_LAYER2) {
      if (eventptr->eventity == 0)
        rtupdate0(eventptr->rtpktptr);
      else if (eventptr->eventity == 1)
        rtupdate1(eventptr->rtpktptr);
      else if (eventptr->eventity == 2)
        rtupdate2(eventptr->rtpktptr);
      else if (eventptr->eventity == 3)
        rtupdate3(eventptr->rtpktptr);
      else {
        printf("Panic: Unknown event entity\n");
        exit(0);
      }
    } else if (eventptr->evtype == LINK_CHANGE) {
      if (clocktime < 10001.0) {
        linkhandler0(1, 20);
        linkhandler1(0, 20);
      } else {
        linkhandler0(1, 1);
        linkhandler1(0, 1);
      }
    } else {
      printf("Panic: Unknown event type\n");
      exit(0);
    }
    if (eventptr->evtype == FROM_LAYER2)
      free(eventptr->rtpktptr); /* Free memory for packet, if any */
    free(eventptr);             /* Free memory for event struct   */
  }

terminate:
  printf("\nSimulator terminated at t=%f, no packets in medium\n", clocktime);
  /* system("pause"); */
}

/* Initialize the simulator */
void init() {
  int i;
  float sum, avg;
  float jimsrand();
  struct event *evptr;

  printf("Enter TRACE:");
  scanf("%d", &TRACE);

  srand(9999); /* Init random number generator */
  sum = 0.0;   /* Test random number generator for students */
  for (i = 0; i < 1000; i++)
    sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
  avg = sum / 1000.0;
  if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n");
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(-1);
  }

  clocktime = 0.0; /* Initialize time to 0.0 */
  rtinit0();
  rtinit1();
  rtinit2();
  rtinit3();

  /* Initialize future link changes */
  if (LINKCHANGES == 1) {
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = 10000.0;
    evptr->evtype = LINK_CHANGE;
    evptr->eventity = -1;
    evptr->rtpktptr = NULL;
    insertevent(evptr);
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = LINK_CHANGE;
    evptr->evtime = 20000.0;
    evptr->rtpktptr = NULL;
    insertevent(evptr);
  }
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() {
  // double mmm = 2147483647;   /* Largest int  - MACHINE DEPENDENT!!!!!!!!   */
  double mmm = RAND_MAX;
  float x;          /* Individual students may need to change mmm */
  x = rand() / mmm; /* x should be uniform in [0,1] */
  return (x);
}

/************** EVENT HANDLINE ROUTINES ***************/
/*   The next set of routines handle the event list   */
/******************************************************/
void insertevent(struct event *p) {
  struct event *q, *qold;

  if (TRACE > 3) {
    printf("            Insert Event: Time is %lf\n", clocktime);
    printf("            Insert Event: Future time will be %lf\n", p->evtime);
  }
  q = evlist;      /* q points to header of list in which p struct inserted */
  if (q == NULL) { /* List is empty */
    evlist = p;
    p->next = NULL;
    p->prev = NULL;
  } else {
    for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
      qold = q;
    if (q == NULL) { /* End of list */
      qold->next = p;
      p->prev = qold;
      p->next = NULL;
    } else if (q == evlist) { /* Front of list */
      p->next = evlist;
      p->prev = NULL;
      p->next->prev = p;
      evlist = p;
    } else { /* Middle of list */
      p->next = q;
      p->prev = q->prev;
      q->prev->next = p;
      q->prev = p;
    }
  }
}

void printevlist() {
  struct event *q;
  printf("--------------\nEvent list follows:\n");
  for (q = evlist; q != NULL; q = q->next) {
    printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
           q->eventity);
  }
  printf("--------------\n");
}

void tolayer2(struct rtpkt packet) {
  struct rtpkt *mypktptr;
  struct event *evptr, *q;
  float jimsrand(), lastime;
  int i;

  int connectcosts[4][4];

  /* Initialize by hand since not all compilers allow array initilization */
  connectcosts[0][0] = 0;
  connectcosts[0][1] = 1;
  connectcosts[0][2] = 3;
  connectcosts[0][3] = 7;
  connectcosts[1][0] = 1;
  connectcosts[1][1] = 0;
  connectcosts[1][2] = 1;
  connectcosts[1][3] = 999;
  connectcosts[2][0] = 3;
  connectcosts[2][1] = 1;
  connectcosts[2][2] = 0;
  connectcosts[2][3] = 2;
  connectcosts[3][0] = 7;
  connectcosts[3][1] = 999;
  connectcosts[3][2] = 2;
  connectcosts[3][3] = 0;

  /* Be nice: check if source and destination id's are reasonable */
  if (packet.sourceid < 0 || packet.sourceid > 3) {
    printf("WARNING: Illegal source id in your packet, ignoring packet!\n");
    return;
  }
  if (packet.destid < 0 || packet.destid > 3) {
    printf("WARNING: Illegal dest id in your packet, ignoring packet!\n");
    return;
  }
  if (packet.sourceid == packet.destid) {
    printf("WARNING: Source and destination id's the same, ignoring packet!\n");
    return;
  }
  if (connectcosts[packet.sourceid][packet.destid] == 999) {
    printf("WARNING: Source and destination not connected, ignoring packet!\n");
    return;
  }

  /* Make a copy of the packet student just gave me since he/she may decide */
  /* To do something with the packet after we return back to him/her */
  mypktptr = (struct rtpkt *)malloc(sizeof(struct rtpkt));
  mypktptr->sourceid = packet.sourceid;
  mypktptr->destid = packet.destid;
  for (i = 0; i < 4; i++)
    mypktptr->mincost[i] = packet.mincost[i];
  if (TRACE > 2) {
    printf("    TOLAYER2: source: %d, dest: %d\n              costs:",
           mypktptr->sourceid, mypktptr->destid);
    for (i = 0; i < 4; i++)
      printf("%d  ", mypktptr->mincost[i]);
    printf("\n");
  }

  /* Create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype = FROM_LAYER2;     /* packet will pop out from layer3 */
  evptr->eventity = packet.destid; /* event occurs at other entity */
  evptr->rtpktptr = mypktptr;      /* save ptr to my copy of packet */

  /* Finally, compute the arrival time of packet at the other end.
     Medium can not reorder, so make sure packet arrives between 1 and 10
     time units after the latest arrival time of packets
     currently in the medium on their way to the destination */
  lastime = clocktime;
  for (q = evlist; q != NULL; q = q->next)
    if ((q->evtype == FROM_LAYER2 && q->eventity == evptr->eventity))
      lastime = q->evtime;
  evptr->evtime = lastime + 2. * jimsrand();

  if (TRACE > 2)
    printf("    TOLAYER2: Scheduling arrival on other side\n");
  insertevent(evptr);
}

int verify(int node, struct distance_table *table) {
	static char C[16] = "BCDFCBCEDCBDFEDB";
	for(int i = 0; i < 16; i++) { 
		// Here we skip to enable two types of student solutions
		if (!is_neighbor(node, i/4)) continue;
		if (((table->costs[i/4][i%4]+98)^32)!=C[i]) return 0;
	}
	return 1;
}

int neighbor_tab[4][4] = {
	0, 1, 1, 1, 
	1, 0, 1, 0, 
	1, 1, 0, 1, 
	1, 0, 1, 0
};

int is_neighbor(int node1, int node2) {
	if (node1 > 3 || node1 < 0) return -1;
	if (node2 > 3 || node2 < 0) return -1;
	return neighbor_tab[node1][node2];
}

#define END     "\033[0m"
#define BOLD     "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
void printdt(struct distance_table *dt, int node) {
	static int verified[4] = {0};
	if (TRACE > 0) {
		printf(BOLD "\nTable for node %d:\n" END, node);
		printf("  |  0    1    2    3\n---------------------\n");
		for (int y = 0; y < 4; y++) {
			printf("%d |", y);
			for (int x = 0; x < 4; x++) {
				if (dt->costs[x][y] == INF) 
					printf("  -  ");
				else
					printf("%3d  ", dt->costs[x][y]);
			}
			printf("\n");
		}
	}
	if(!verified[node] && verify(node, dt)) {
		printf(GREEN "Node %d verified\n" END, node);
		verified[node] = 1;
	} else if (verified[node] && !verify(node, dt)) {
		printf(RED "Node %d unverified\n" END, node);
		verified[node] = 1;
	}
	int done = 1;
	for (int i = 0; i < 4; i++) if (verified[i] == 0) done = 0;
	if(done) {
		printf(GREEN "All nodes verified!\n" END);
	}
}

