#https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_C_libraries.html

OUT=libminiOS.so
CFLAGS=-Wall
CC=gcc-11
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	 (RM) $ (OUT)
