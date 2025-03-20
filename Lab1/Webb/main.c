#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUF_SIZE 4096		 /* block transfer size */
#define QUEUE_SIZE 10
#define BODY_MAX_SIZE 1024
#define RESPONSE_MAX_SIZE 1024

static void handle_response(int socket_descriptor, char *r_type, char *c_type,
														char *body, long body_size) {
	char result[RESPONSE_MAX_SIZE];
	snprintf(result, RESPONSE_MAX_SIZE,
					 "HTTP/1.1 %s\r\n"
					 "Server: Web Server\r\n"
					 "Content-Length: %li\r\n"
					 "Content-Type: %s\r\n\r\n",
					 r_type, body_size, c_type);
	send(socket_descriptor, result, strlen(result), 0);

	send(socket_descriptor, body, body_size, 0);
}

static void parse_request(char buf[BUF_SIZE], char out_path[BUF_SIZE]) {
	char buffer[BUF_SIZE];
	memcpy(buffer, buf, BUF_SIZE);

	char *path = strtok(buffer, " ");
	path = strtok(NULL, " ");

	assert(path[0] == '/');
	if (strcmp(path, "/") == 0) {
		path = "/index.html";
	}

	// "/index.html" -> "CURRENT_PATH/sample_website/index.html"
	snprintf(out_path, BUF_SIZE, "./sample_website%s", path);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in channel; /* holds IP address */
	/* Build address structure to bind to socket. */
	memset(&channel, 0, sizeof(channel)); /* zero channel */
	channel.sin_family = AF_INET;
	channel.sin_addr.s_addr = htonl(INADDR_ANY);
	channel.sin_port = htons(SERVER_PORT);

	/* Passive open. Wait for connection. */
	/* create socket */
	int socket_result = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_result < 0) {
		printf("socket failed\n");
		exit(1);
	}

	int on = 1;
	setsockopt(socket_result, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	int bind_result =
			bind(socket_result, (struct sockaddr *)&channel, sizeof(channel));
	if (bind_result < 0) {
		printf("bind failed\n");
		exit(1);
	}

	/* specify queue size */
	int listen_result = listen(socket_result, QUEUE_SIZE);
	if (listen_result < 0) {
		printf("listen failed\n");
		exit(1);
	}

	/* Socket is now set up and bound. Wait for connection and process it. */
	while (1) {
		int socket_descriptor =
				accept(socket_result, 0, 0); /* block for connection request */
		if (socket_descriptor < 0) {
			printf("accept failed\n");
			exit(1);
		}

		/* read file name from socket */
		struct sockaddr_in client_addr;
		// "should be initialized to the size of the buffer associated with
		// src_addr" - man 2 recv
		socklen_t addr_len = sizeof(client_addr);
		char buf[BUF_SIZE];
		recvfrom(socket_descriptor, buf, sizeof(buf), 0,
						 (struct sockaddr *)&client_addr, &addr_len);

		/* Get and return the file. */
		char path[BUF_SIZE];
		parse_request(buf, path);

		// open the file to be sent back
		int fd = open(path, O_RDONLY);
		if (fd < 0) {
			// 404
			char body[] = "404 Not Found";
			handle_response(socket_descriptor, "404 Not Found", "text/plain", body,
											strlen(body));
		} else {
			// 200
			char *type = "text/plain";
			char *file_extension = strrchr(path, '.') + 1;
			if (strcmp(file_extension, "html") == 0) {
				type = "text/html";
			} else if (strcmp(file_extension, "jpg") == 0) {
				type = "image/jpeg";
			} else if (strcmp(file_extension, "png") == 0) {
				type = "image/png";
			}

			// Seek to the end to get the size in bytes
			off_t body_size = lseek(fd, 0, SEEK_END);
			// Go back to the start
			lseek(fd, 0, SEEK_SET);
			// Read whole file
			char *body = malloc(body_size + 1);
			read(fd, body, body_size);

			handle_response(socket_descriptor, "200 OK", type, body, body_size);

			free(body);
		}

		close(fd);
		close(socket_descriptor);
	}
}
