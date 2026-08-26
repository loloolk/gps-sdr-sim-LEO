# Makefile for Linux etc.

# gcc appends .exe on Windows, so the link target has to carry the suffix too.
# Without it make never sees the file it just built and relinks on every run.
ifeq ($(OS),Windows_NT)
EXE=.exe
endif
BIN=gps-sdr-sim$(EXE)

.PHONY: all clean time check-gcc
all: check-gcc $(BIN)

SHELL=/bin/bash
CC=gcc
CFLAGS=-O3 -Wall -std=gnu2x -D_FILE_OFFSET_BITS=64
ifdef USER_MOTION_SIZE
CFLAGS+=-DUSER_MOTION_SIZE=$(USER_MOTION_SIZE)
endif
LDFLAGS=-lm

# --- NEW: Compiler version check ---
check-gcc:
	@if $(CC) --version | grep -qi "gcc"; then \
		GCC_MAJOR=$$($(CC) -dumpversion | cut -d. -f1); \
		if [ "$$GCC_MAJOR" -lt 13 ]; then \
			echo -e "\033[0;31mERROR: GCC 13 or higher is required for C23 constexpr support.\033[0m"; \
			echo "Detected GCC version: $$($(CC) -dumpversion)"; \
			exit 1; \
		fi; \
	fi

$(BIN): gpssim.o
	${CC} $< ${LDFLAGS} -o $@

gpssim.o: .user-motion-size gpssim.h

.user-motion-size: .FORCE
	@if [ -f .user-motion-size ]; then \
		if [ "`cat .user-motion-size`" != "$(USER_MOTION_SIZE)" ]; then \
			echo "Updating .user-motion-size"; \
			echo "$(USER_MOTION_SIZE)" >| .user-motion-size; \
		fi; \
	else \
		echo "$(USER_MOTION_SIZE)" > .user-motion-size; \
	fi;

clean:
	rm -f gpssim.o gps-sdr-sim gps-sdr-sim.exe *.bin .user-motion-size

# Inputs for the `time` benchmark; override on the command line if needed.
BENCH_EPH?=input_files/ephemeris/brdc0010.22n
BENCH_MOTION?=input_files/circle.csv

# `time` is a bash keyword, not a binary, and make execs a recipe line with no
# shell metacharacters directly instead of handing it to $(SHELL). The loop
# keeps the recipe off that fast path so bash gets to interpret it.
time: $(BIN)
	@for bits in 1 8 16; do \
		echo "=== $(BIN) -b $$bits ==="; \
		time ./$(BIN) -e $(BENCH_EPH) -u $(BENCH_MOTION) -b $$bits || exit 1; \
	done

.FORCE:

YEAR?=$(shell date +"%Y")
Y=$(patsubst 20%,%,$(YEAR))
%.$(Y)n:
	wget -q ftp://cddis.gsfc.nasa.gov/gnss/data/daily/$(YEAR)/brdc/$@.Z -O $@.Z
	uncompress $@.Z