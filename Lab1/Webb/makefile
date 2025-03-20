src = $(wildcard *.c)
obj = $(src:.c=.o)

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

web_server: $(obj)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(obj) web_server
