OUT=test
CFLAGS=-Wall
CC=gcc-11
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	cp ../libminiOS.so libminiOS.so
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -fsanitize=address -fno-omit-frame-pointer -O2 -g

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< -fsanitize=address 

clean :
	$(RM) $(OFILES)
	
mrproper : clean
	 (RM) $ (OUT)

