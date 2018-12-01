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

DYNAMIC_LIBS_DIR = -L./xml/lib -L./libs -L./openssl/lib
DYNAMIC_LIBS = -lsdkxml -lsdkz #-lcrypto -lssl
STATIC_LIBS = ./openssl/lib/libcrypto.a ./openssl/lib/libssl.a

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
           -I./time \
           -I./algorithm \
           -I./xml/inc \
           -I./xml/include \
           -I./thread \
           -I./includes \
           -I./zlib/contrib/minizip \
           -I./openssl/include \
           -I./network           
           
#SOURCES = $(wildcard *.c)
SOURCES = $(wildcard */*.c)
SOURCES += $(wildcard */src/*.c)
SRC_DIR = $(SOURCES)

#OBJ = $(TARGET).o
#OBJ += $(INC:%.h=%.o)
CFLAGS += $(HEAD_DIR)
#CFLAGS += $(DYNAMIC_LIBS_DIR)
#CFLAGS += $(DYNAMIC_LIBS)
#CFLAGS += $(STATIC_LIBS)

OBJ += $(SRC_DIR:%.c=%.o)
$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(DYNAMIC_LIBS_DIR) $(DYNAMIC_LIBS) $(STATIC_LIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm -f *.o *.out *.elf ./*/*.o ./*/*.swo* ./*/*.swp*
	rm -rf $(OBJ)






