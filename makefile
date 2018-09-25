#/**************************************************
#*file name         :makefile
#*descrption        : 
#*author            :pengyicheng
#*date              :20161125
#*version           :1.0
#***************************************************/ 

CFLAGS += -g -fPIC
CFLAGS += -I$(shell pwd) 
CFLAGS += -O2 -pthread
ifeq ($(COMPILE_TYPE),shared)
    CFLAGS += -shared 
else
    OBJ = $(TARGET).o 
endif

DYNAMIC_LIBS = 

CUR_PWD=$(shell pwd)

export HEAD_DIR = -I./mempool \
           -I./string \
           -I./example \
           -I./osadapter \
           -I./string \
           -I./log \
           -I./file \
           -I./json \
           -I./test \
           -I./time

SRC_DIR = $(CUR_PWD)/example/mempool_example.c \
          $(CUR_PWD)/mempool/mempool.c \
          $(CUR_PWD)/osadapter/osadapter.c \
          $(CUR_PWD)/string/utils_string.c \
          $(CUR_PWD)/log/commons_log.c \
          $(CUR_PWD)/file/utils_file.c \
          $(CUR_PWD)/json/cJSON.c \
          $(CUR_PWD)/time/utils_time.c 

#OBJ = $(TARGET).o
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
	rm -f *.o *.out *.elf ./*/*.o ./*/*.swo* ./*/*.swp*






