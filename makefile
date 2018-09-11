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

CUR_PWD=$(shell pwd)

HEAD_DIR = -I./mempool \
           -I./string \
           -I./example \
           -I./type \
           -I./osadapter \
           -I./string

SRC_DIR = $(CUR_PWD)/example/mempool_example.c \
          $(CUR_PWD)/mempool/mempool.c \
          $(CUR_PWD)/osadapter/osadapter.c \
          $(CUR_PWD)/string/utils_string.c

OBJ = $(TARGET).o
#OBJ += $(INC:%.h=%.o)
CFLAGS += $(HEAD_DIR)
OBJ += $(SRC_DIR:%.c=%.o)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *.out






