OUT=test
CFLAGS=-Wall
CC=gcc
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT) test_hm

test_hm : test_hm.o
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2
	
$(OUT): $(OFILES)
	cp ../libminiOS.so libminiOS.so
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2

%.o: %.c
	$(CC) -g $(CFLAGS) -o $@ -c $<

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	$(RM) $(OUT)

