#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./constants.h"

int main() {
	// Create a UDP socket
	int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_fd < 0) {
		perror("Socket creation failed");
		exit(1);
	}

	// Setup server address structure
	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// Send an empty request to the server
	sendto(socket_fd, NULL, 0, 0, (struct sockaddr *)&server_addr,
				 sizeof(server_addr));

	// Receive the server's response
	uint32_t server_time;
	recv(socket_fd, &server_time, sizeof(server_time), 0);

	server_time = ntohl(server_time);
	// Convert RFC 868 time back to Unix time
	time_t unix_time = server_time - SEVENTY_LONG_YEARS;

	printf("Server time: %s", ctime(&unix_time));

	close(socket_fd);
	return 0;
}
