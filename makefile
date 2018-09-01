#/**************************************************
#*file name         :makefile
#*descrption        : 
#*author            :pengyicheng
#*date              :20161125
#*version           :1.0
#***************************************************/ 

CFLAGS += -g
CFLAGS := -I$(shell pwd) -Wall 
CFLAGS += -O2 
#CFLAGS += -I/usr/include/sys
OBJ = $(TARGET).o
OBJ += $(INC:%.h=%.o)
#SRC = *.c
#OBJ += $(SRC:%.c=%.o)

$(TARGET):$(OBJ)
	gcc $(CFLAGS) $(OBJ) -o $(TARGET)

%.o : %.c
	gcc -c $(CFLAGS) $< #-o $@

.PHONY:clean
clean:
	rm -f *.o *.out






