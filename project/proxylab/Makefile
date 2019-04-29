#
# Makefile for Proxy Lab
#
# You need to modify this file to compile and link any additional files
# required for your proxy. Don't modify the file outside of the designated
# section, unless you know what you're doing.
#
# Autolab will execute the command "make proxy" on your specific
# Makefile to build your proxy from sources.
#
CC = gcc
CFLAGS = -g -Og -Wall -std=c99 -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
LDLIBS = -lpthread

# Uncomment this to enable debug macros
# CFLAGS += -DDEBUG

# Targets to compile
FILES = proxy handin

# Default build rule
.PHONY: all
all: $(FILES) tiny-code

########################
# Begin modifying here #
########################

# List of all header files
DEPS = csapp.h

# Rules for building proxy
proxy: proxy.o csapp.o
proxy.o: proxy.c $(DEPS)
csapp.o: csapp.c $(DEPS)

######################
# End modifying here #
######################

.PHONY: tiny-code
tiny-code:
	(cd tiny; make)

# Creates a tarball in proxylab-handin.tar that you can hand in to Autolab.
# If you have additional files you want to ignore, do NOT modify this command;
# instead, add them to the .tarignore file.
.PHONY: handin
handin:
	tar cvf proxylab-handin.tar --exclude-from=.gitignore \
		--exclude-from=.tarignore *

.PHONY: clean
clean:
	rm -f *~ *.o core $(FILES)
	rm -rf logs source_files response_files results.log get_files
	(cd tiny; make clean)
