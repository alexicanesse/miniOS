OUT=test
CFLAGS=-Wall
CC=gcc
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: init $(OUT) test_hm

init :
	cp ../libminiOS.so libminiOS.so

test_hm : test_hm.o
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2
	
$(OUT): $(OFILES)
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	$(RM) $(OUT)

