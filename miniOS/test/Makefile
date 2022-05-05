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


OUT=test
CFLAGS=-Wall
CC=gcc
#LDFLAGS =
CFILES=$(wildcard *.c)
OFILES=$(CFILES:.c=.o)

all: init $(OUT) test_hm

init :
	@echo "${PURPLE}Compiling main project${RESET}"
	cd .. && make
	@echo "${PURPLE}Copying library file${RESET}"
	cp ../libminiOS.so libminiOS.so

test_hm : test_hm.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	
$(OUT): $(OFILES)
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o $@ $^ $(LDFLAGS) -L. -lminiOS -O2 -g

%.o: %.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g

clean :
	@echo "${RED}Cleaning${RESET}"
	$(RM) $(OFILES)
	
mrproper : clean
	@echo "${RED}Cleaning all files${RESET}"
	$(RM) $(OUT) test_hm libminiOS.so

