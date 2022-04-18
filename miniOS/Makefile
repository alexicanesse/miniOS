#https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_C_libraries.html

OUT=libminiOS.so
CFLAGS=-Wall -D_POSIX_C_SOURCE=199309L -fPIC
CC=gcc
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm -fPIC

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	 (RM) $ (OUT)
