#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sim_Engine.h"
#include "util.h"

// Sender

static struct pkt *current_packet = NULL;
static queue_t queue = NULL;
static int seq = 0;

static inline void send_packet(struct pkt *packet) {
	tolayer3(A, *packet);
	starttimer(A, TIMEOUT);
}

/* Called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message) {
	if (current_packet) {
		// Add the message to the queue
		queue = queue_enqueue(queue, &message);
		debug("A Out: still waiting for ACK, queueing: %s\n", message.data);
		return;
	}

	debug("A Out: Sending message to receiver with message: %s\n", message.data);

	current_packet = calloc(1, sizeof(*current_packet));
	current_packet->acknum = NACK;
	current_packet->checksum = hash(message.data);
	current_packet->seqnum = seq;
	strncpy(current_packet->payload, message.data, DATA_SIZE);

	send_packet(current_packet);
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {
	if (!current_packet) {
		// Caused by when the timeout delay and the package sim intensity
		// don't match
		debug("A In: No current packet in transit, returning");
		return;
	}

	stoptimer(A);

	const int checksum = hash(packet.payload);
	const bool matches = checksum == packet.checksum;
	if (packet.seqnum != seq || packet.acknum == NACK || !matches) {
		// NACK/corrupt: Retransmit packet
		debug("A In: retransmitting (NACK: %s, Corrupt: %s): %s\n",
					btos(packet.acknum == NACK), btos(!matches), packet.payload);
		send_packet(current_packet);
		return;
	}

	///
	/// ACK
	///

	debug("A In: ACK:ed\n");
	seq = (seq + 1) % 2;
	free(current_packet);
	current_packet = NULL;

	// Transmit next from queue
	if (queue) {
		struct msg *message = NULL;
		queue = queue_dequeue(queue, (void **)&message);
		debug("A In: sending from queue: %s\n", message->data);
		A_output(*message);
		free(message);
	}
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
	debug("A: Timer interrupt, retransmitting: %s\n", current_packet->payload);
	send_packet(current_packet);
}

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
	// no-op
}
