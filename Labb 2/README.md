# Sim

- Erik Gunnar Reider
- Daniel Brynne

## Running:

Regularly:

```sh
make clean # To clear eventual per-target defined macros
make
./sim PACKETS LOSS_RATE CORRUPTION_RATE INTENSITY TRACE_LEVEL
```

Debug:

```sh
make clean # To clear eventual per-target defined macros
make debug
./sim PACKETS LOSS_RATE CORRUPTION_RATE INTENSITY TRACE_LEVEL
```
