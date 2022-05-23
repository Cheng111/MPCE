# Makefile for Clique

CC = gcc
#CC = g++
HEAD = head
OBJDIR = obj
SDIR = src
CFLAGS = -O3 -g -Wall -I$(HEAD)
PTHREADS = -lpthread
TESTFLAGS = #-DPERFORMANCE -DDEBUG
LFSFLAG = -D_FILE_OFFSET_BITS=64

OBJECTS = clique-driver.o graph.o utility.o bk.o ReadKG.o
HEADERS = graph.h utility.h bk.h ReadKG.h
EXECUTABLE = mpce

all: $(EXECUTABLE)

#pmce: clique-driver.o $(OBJECTS) #$(HEADERS) Makefile
$(EXECUTABLE): $(OBJECTS) #$(HEADERS) Makefile
	mv $(OBJECTS) $(OBJDIR)/
	$(CC) $(CFLAGS) $(LFSFLAG) -o $@ $(OBJDIR)/*.o $(PTHREADS)

#.c.o:
$(OBJECTS):
#	echo $(CC) $(CFLAGS) $(LFSFLAG) $(TESTFLAGS) -c $(SDIR)/*.c
	$(CC) $(CFLAGS) $(LFSFLAG) $(TESTFLAGS) -c $(SDIR)/*.c
#	mv $(OBJECTS) $(OBJDIR)/

clean:
	rm -f core* *.o $(EXECUTABLE) gmon.out gprof.out $(OBJDIR)/*

