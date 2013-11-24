# Makefile for HWP Sound Seperation
# Author : Weipeng He

all : capture readdata

LIBS = -lasound
FLAGS = -Wall

debug : FLAGS += -g
debug : all 

capture : capalsa.c
	$(CC) $(FLAGS) $< -o $@ $(LIBS)

readdata : readdata.c
	$(CC) $(FLAGS) $< -o $@ $(LIBS)

.PHONY : clean
clean :
	$(RM) capture

