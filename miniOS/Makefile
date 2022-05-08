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
CFLAGS=-Wall -fPIC -I./include
#CFLAGS=-Wall -D_POSIX_C_SOURCE=199309L -fPIC
CC=gcc
LDFLAGS = -pthread

FILES = scheduler vCPU memory uThread_tree uThread_queue miniOS
CFILES = $(addsuffix .c, $(addprefix ./src/, $(FILES)))
OFILES = $(addsuffix .o, $(addprefix ./objects/, $(FILES)))


all: $(OUT)
	

$(OUT): $(OFILES)
	@echo "${BLUE}Linking C library" $@ "${RESET}"
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm -fPIC -g

./objects/vCPU.o : ./src/vCPU.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) -D_POSIX_C_SOURCE=199309L $(CFLAGS) -o $@ -c $< -g

./objects/%.o: ./src/%.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g

clean :
	@echo "${RED}Cleaning${RESET}"
	$(RM) $(OFILES)
	
mrproper : clean
	@echo "${RED}Cleaning all files${RESET}"
	$(RM) $(OUT)
