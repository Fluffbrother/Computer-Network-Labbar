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

static void parse_request(char buf[BUF_SIZE]) {
	char *path = strtok(buf, " ");
	path = strtok(NULL, " ");

	assert(path[0] == '/');
	if (strcmp(path, "/") == 0) {
		path = "/index.html";
	}

	// "/index.html" -> "CURRENT_PATH/sample_website/index.html"
	snprintf(buf, BUF_SIZE, "./sample_website%s", path);
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
	int fd, sa, bytes;
	char buf[BUF_SIZE]; /* buffer for outgoing file */
	while (1) {
		sa = accept(socket_result, 0, 0); /* block for connection request */
		if (sa < 0) {
			printf("accept failed\n");
			exit(1);
		}
		/* read file name from socket */
		read(sa, buf, BUF_SIZE);

		/* Get and return the file. */
		parse_request(buf);
		printf("PATH: %s\n", buf);

		/* open the file to be sent back */
		fd = open(buf, O_RDONLY);
		if (fd < 0) {
			// TODO: 404
			printf("open failed: %s\n", buf);
			exit(1);
		}
		while (1) {
			bytes = read(fd, buf, BUF_SIZE); /* read from file */
			if (bytes <= 0) {
				break; /* check for end of file */
			}
			write(sa, buf, bytes); /* write bytes to socket */
		}
		close(fd); /* close file */
		close(sa); /* close connection */
	}
}
