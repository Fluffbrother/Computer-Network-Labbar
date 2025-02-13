#include "Sim_Engine.h"


/* Called from layer 5, passed the data to be sent to other side */
void A_output( struct msg message) {
  /* TODO */
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {
  /* TODO */
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
  /* TODO */
}  

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
  /* TODO */
}
