#https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_C_libraries.html

#define colors for nice output
ifneq (,$(findstring xterm,${TERM}))
	RED          := $(shell tput -Txterm setaf 1)
	PURPLE       := $(shell tput -Txterm setaf 5)
	BLUE         := $(shell tput -Txterm setaf 6)
	RESET := $(shell tput -Txterm sgr0)
else
	RED       := ""
	PURPLE       := ""
	BLUE         := ""
	RESET        := ""
endif



OUT=libminiOS.so
CFLAGS=-Wall -fPIC
#CFLAGS=-Wall -D_POSIX_C_SOURCE=199309L -fPIC
CC=gcc
LDFLAGS = -pthread
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: $(OUT)

$(OUT): $(OFILES)
	@echo "${BLUE}Linking C library" $@ "${RESET}"
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm -fPIC -g

vCPU.o : vCPU.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) -Wall -D_POSIX_C_SOURCE=199309L -fPIC -o $@ -c $< -g

%.o: %.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g

clean :
	@echo "${RED}Cleaning${RESET}"
	$(RM) $(OFILES)
	
mrproper : clean
	@echo "${RED}Cleaning all files${RESET}"
	$(RM) $(OUT)
