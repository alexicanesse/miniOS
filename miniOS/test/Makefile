OUT=test
CFLAGS=-Wall
CC=gcc
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	cp ../libminiOS.so libminiOS.so
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	 (RM) $ (OUT)

