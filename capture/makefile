# Makefile for HWP Sound Seperation
# Author : Weipeng He

all : capture

LIBS = -lasound
FLAGS = -Wall

capture : capalsa.c
	$(CC) $(FLAGS) $< -o $@ $(LIBS)

