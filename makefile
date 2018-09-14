#/**************************************************
#*file name         :makefile
#*descrption        : 
#*author            :pengyicheng
#*date              :20161125
#*version           :1.0
#***************************************************/ 

CFLAGS += -g
CFLAGS := -I$(shell pwd) 
CFLAGS += -O2

DYNAMIC_LIBS = 

CUR_PWD=$(shell pwd)

HEAD_DIR = -I./mempool \
           -I./string \
           -I./example \
           -I./osadapter \
           -I./string \
           -I./log \
           -I./file

SRC_DIR = $(CUR_PWD)/example/mempool_example.c \
          $(CUR_PWD)/mempool/mempool.c \
          $(CUR_PWD)/osadapter/osadapter.c \
          $(CUR_PWD)/string/utils_string.c \
          $(CUR_PWD)/log/commons_log.c \
          $(CUR_PWD)/file/utils_file.c

OBJ = $(TARGET).o
#OBJ += $(INC:%.h=%.o)
CFLAGS += $(HEAD_DIR)
CFLAGS += $(DYNAMIC_LIBS)

OBJ += $(SRC_DIR:%.c=%.o)
$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *.out *.elf ./*/*.o






