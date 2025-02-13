#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PORT 37 // RFC 868 Time Protocol Port
#define BUFFER_SIZE 1

uint32_t get_time() {
	time_t current_time = time(NULL);
	return htonl((uint32_t)(current_time +
													2208988800U)); // Convert Unix time to RFC 868 time
}

int main() {
	int sockfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE]; // Placeholder for an empty request
	uint32_t response_time;

	// Create a UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Setup server address structure
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind the socket to the port
	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// printf("Time server running on UDP port %d...\n", PORT);

	while (1) {
		// Receive request from a client
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr,
						 &addr_len);

		// Get the RFC 868 time and send it to the client
		response_time = get_time();
		sendto(sockfd, &response_time, sizeof(response_time), 0,
					 (struct sockaddr *)&client_addr, addr_len);

		// printf("Sent time to client: %s", ctime((time_t *)&response_time));
	}

	close(sockfd);
	return 0;
}
