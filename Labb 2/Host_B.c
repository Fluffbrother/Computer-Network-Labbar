#include <stdbool.h>

#include "Sim_Engine.h"
#include "util.h"

// Receiver

static int seq = 0;

/* Called from layer 5, passed the data to be sent to other side */
void B_output(struct msg message) {
	/* DON'T IMPLEMENT */
}

/* Called from layer 3, when a packet arrives for layer 4 */
void B_input(struct pkt packet) {
	const int checksum = hash(packet.payload);
	const bool matches = checksum == packet.checksum;

	debug("B in: Received payload: %s, checksum matches: %s\n", packet.payload,
				btos(matches));

	if (matches) {
		// Send ACK
		debug("B in: Sending ACK!: Package-Seq: %i, Seq: %i\n", packet.seqnum, seq);
		packet.acknum = ACK;
		tolayer3(B, packet);

		if (packet.seqnum == seq) {
			// Send to layer 5
			debug("B in: Sending to layer 5\n");
			tolayer5(B, packet.payload);

			seq = (seq + 1) % 2;
		}
	} else {
		// Send NACK
		debug("B in: Checksum doesn't match, sending NACK\n");
		packet.acknum = NACK;
		tolayer3(B, packet);
	}
}

/* Called when B's timer goes off */
void B_timerinterrupt() {
	// no-op
}

/* The following routine will be called once (only) before any other */
/* Host B routines are called. You can use it to do any initialization */
void B_init() {
	// no-op
}
