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

sim: $(obj)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: debug
debug: CFLAGS += -DDEBUGGING=1
debug: $(obj)
	$(CC) $(CFLAGS) -o sim $^

.PHONY: clean
clean:
	rm -f $(obj) sim

