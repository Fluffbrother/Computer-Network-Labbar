#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./constants.h"

/** Convert Unix time to RFC 868 time by adding 70 years (1970 -> 1900) */
static inline uint32_t get_rfc_time() {
	time_t current_time = time(NULL);
	return htonl(current_time + SEVENTY_LONG_YEARS);
}

int main() {
	// Create a UDP socket
	int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_fd < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Setup server address structure
	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);

	// Bind the socket to the port
	int bind_result =
			bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (bind_result < 0) {
		perror("Bind failed");
		close(socket_fd);
		exit(EXIT_FAILURE);
	}

	while (true) {
		struct sockaddr_in client_addr;
		// "should be initialized to the size of the buffer associated with
		// src_addr" - man 2 recv
		socklen_t addr_len = sizeof(client_addr);

		// Receive *empty* request from a client
		recvfrom(socket_fd, NULL, 0, 0, (struct sockaddr *)&client_addr, &addr_len);

		printf("Connection from: %s:%i\n", inet_ntoa(client_addr.sin_addr),
					 client_addr.sin_port);

		// Get the RFC 868 time and send it to the client
		uint32_t time = get_rfc_time();
		sendto(socket_fd, &time, sizeof(time), 0, (struct sockaddr *)&client_addr,
					 addr_len);
	}

	close(socket_fd);
	return 0;
}
