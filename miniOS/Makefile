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

FILES = scheduler vCPU memory memory_legacy uThread_tree uThread_queue miniOS
CFILES = $(addsuffix .c, $(addprefix ./src/, $(FILES)))
OFILES = $(addsuffix .o, $(addprefix ./objects/, $(FILES)))
DIRECTORIES = ./objects

$(DIRECTORIES) :
	@echo "${PURPLE}Creating missing directory" $@ "${RESET}"
	mkdir $@

all: $(OUT)

$(OUT): $(OFILES)
	@echo "${BLUE}Linking C library" $@ "${RESET}"
	$(CC) -o $@ $^ $(LDFLAGS) -shared -lm -fPIC -g

./objects/vCPU.o : ./src/vCPU.c $(DIRECTORIES)
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) -D_POSIX_C_SOURCE=199309L $(CFLAGS) -o $@ -c $< -g

./objects/%.o: ./src/%.c $(DIRECTORIES)
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g
	
test_hm_all: test_hm test_hm_legacy test_hm_guard_page test_hm_overflow
	
test_hm: $(OUT) ./test/test_hm/test_hm.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o ./test/test_hm/test_hm $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	@echo "${PURPLE}Testing the heap memory manager!\n${RESET}"
	@./test/test_hm/test_hm

./test/test_hm/test_hm.o: ./test/test_hm/test_hm.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g
	
test_hm_overflow: $(OUT) ./test/test_hm/test_hm_overflow.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o ./test/test_hm/test_hm_overflow $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	@echo "${PURPLE}Testing the heap memory manager overflow protection!\n${RESET}"
	@./test/test_hm/test_hm_overflow

./test/test_hm/test_hm.o: ./test/test_hm/test_hm.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g
	
test_hm_guard_page: $(OUT) ./test/test_hm/test_hm_guard_page.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o ./test/test_hm/test_hm_guard_page $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	@echo "${PURPLE}Testing the heap memory manager guard pages!\n${RESET}"
	@./test/test_hm/test_hm_guard_page

./test/test_hm/test_hm_guard_page.o: ./test/test_hm/test_hm_guard_page.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g
	
test_hm_legacy: $(OUT) ./test/test_hm_legacy/test_hm_legacy.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o ./test/test_hm_legacy/test_hm_legacy $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	@echo "${PURPLE}Testing the legacy heap memory manager!\n${RESET}"
	@./test/test_hm_legacy/test_hm_legacy

./test/test_hm_legacy/test_hm_legacy.o: ./test/test_hm_legacy/test_hm_legacy.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g
	
test_vCPU: $(OUT) ./test/test_vCPU/test.o
	@echo "${BLUE}Linking C executable" $@ "${RESET}"
	$(CC) -o ./test/test_vCPU/test_vCPU $^ $(LDFLAGS) -L. -lminiOS -O2 -g
	@echo "${PURPLE}Testing vCPUs!\n\n\n${RESET}"
	./test/test_vCPU/test_vCPU

./test/test_vCPU/test_vCPU.o: ./test/test_vCPU/test.c
	@echo "${PURPLE}Building C object" $@ "${RESET}"
	$(CC) $(CFLAGS) -o $@ -c $< -g

clean :
	@echo "${RED}Cleaning${RESET}"
	$(RM) $(OFILES) ./test/test_hm/test_hm.o
	
mrproper : clean
	@echo "${RED}Cleaning all files${RESET}"
	$(RM) $(OUT) ./test/test_hm/test_hm
