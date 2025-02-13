#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 37

// TODO
#define SERVER_IP "127.0.0.1" // Change this to the actual server IP

int main() {
	int sockfd;
	struct sockaddr_in server_addr;
	char request[1] = {0}; // Empty request
	uint32_t server_time;
	time_t unix_time;

	// Create a UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Setup server address structure
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	// Send an empty request to the server
	sendto(sockfd, request, sizeof(request), 0, (struct sockaddr *)&server_addr,
				 sizeof(server_addr));

	// Receive the server's response
	recvfrom(sockfd, &server_time, sizeof(server_time), 0, NULL, NULL);

	// Convert from RFC 868 time to Unix time
	server_time = ntohl(server_time);
	unix_time = server_time - 2208988800U;

	// Print the received time in a readable format
	printf("Server time: %s", ctime(&unix_time));

	close(sockfd);
	return 0;
}
