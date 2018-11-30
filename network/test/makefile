#!/bin/bash
testThread.elf:testServer.c
	gcc -o testServer.elf testServer.c ../utils_network.c ../../log/commons_log.c -I../ -I./

.PHONY:clean
clean:
	rm -f *.o *.out *.elf ./*/*.o ./*/*.swo* ./*/*.swp*

