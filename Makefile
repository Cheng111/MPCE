# Makefile for Clique

CC = gcc
HEAD = head
OBJDIR = obj
SDIR = src
CFLAGS = -O3 -g -Wall -I$(HEAD)
PTHREADS = -lpthread
TESTFLAGS = # Optional: -DPERFORMANCE -DDEBUG
LFSFLAG = -D_FILE_OFFSET_BITS=64

SRC_FILES = $(wildcard $(SDIR)/*.c)
OBJ_NAMES = $(notdir $(SRC_FILES:.c=.o))
OBJECTS = $(addprefix $(OBJDIR)/, $(OBJ_NAMES))
EXECUTABLE = mpce

.PHONY: all clean debug run

all: $(EXECUTABLE)

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: $(SDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(LFSFLAG) $(TESTFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LFSFLAG) -o $@ $^ $(PTHREADS)

# Debug target to launch with gdb
# Use: `make debug` then type "run" in GDB

debug: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

# Optional target to run directly (useful during development)
run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f core* $(OBJDIR)/*.o $(EXECUTABLE) gmon.out gprof.out

test: $(EXECUTABLE)
	@echo "Running tests with version: $(version)"
	@for file in $(wildcard test/*.txt); do \
		echo "Running ./$(EXECUTABLE) $$file -v $(version) -klb 1 -p"; \
		./$(EXECUTABLE) $$file -v $(version) -klb 1 -p; \
	done
