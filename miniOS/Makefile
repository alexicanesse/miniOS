#https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_C_libraries.html

OUT=libminiOS.so
CFLAGS=-Wall -fPIC
#CFLAGS=-Wall -D_POSIX_C_SOURCE=199309L -fPIC
CC=gcc
LDFLAGS = -pthread
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm -fPIC

vCPU.o : vCPU.c
	$(CC) -Wall -D_POSIX_C_SOURCE=199309L -fPIC -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	$(RM) $(OUT)
