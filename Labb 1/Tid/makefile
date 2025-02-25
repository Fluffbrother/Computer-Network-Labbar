src_client = $(wildcard *client.c)
obj_client = $(src_client:.c=.o)
src_server = $(wildcard *server.c)
obj_server = $(src_server:.c=.o)

CFLAGS = --std=c11 -g -Wextra -Wall \
	-Wno-unused-parameter \
	-Wno-unused-result \
	-Wundef \
	-Wlogical-op \
	-Wpointer-arith \
	-Winit-self \
	-Wstrict-prototypes \
	-Woverflow \
	-Wmissing-prototypes \
	-Walloca

.PHONY: all
all: time_server time_client

time_server: $(obj_server)
	$(CC) $(CFLAGS) -o $@ $^

time_client: $(obj_client)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(obj_server) time_server
	rm -f $(obj_client) time_client
