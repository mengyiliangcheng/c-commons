#!/bin/bash
testThread.elf:testServer.c
	gcc -o testServer.elf testServer.c ../utils_network.c ../../log/commons_log.c ../../thread/utils_sem.c ../../osadapter/osadapter.c  -I../ -I./ -I../../log/ -I../../thread/

.PHONY:clean
clean:
	rm -f *.o *.out *.elf ./*/*.o ./*/*.swo* ./*/*.swp*

