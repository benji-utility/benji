GXX = gcc

GXX_FLAGS = -g

BUILD = build
OBJ = $(BUILD)/obj
EXEC = benji

SRC = $(wildcard src/*.c)
UPDATED_SRC =

ifeq ($(OS), Windows_NT)
	LINKED_LIBS = -lWs2_32 -ldxgi -ldxguid -lole32
	UPDATED_SRC = $(filter-out src/daemon.c, $(SRC))
else ifeq ($(shell uname), Linux)
	LINKED_LIBS =
	UPDATED_SRC = $(filter-out src/service.c, $(SRC))
endif

OBJS = $(subst src/, $(OBJ)/, $(addsuffix .o, $(basename $(UPDATED_SRC))))

all: clean compile

compile: $(BUILD)/$(EXEC)

$(BUILD)/$(EXEC): $(OBJS)
	$(GXX) $(OBJS) -o $@ $(LINKED_LIBS)

$(OBJ)/%.o: src/%.c
	$(GXX) $(GXX_FLAGS) -c $< -o $@

ifeq ($(OS), Windows_NT)
.SILENT: clean
endif

.PHONY: clean
clean: mkbuild
ifeq ($(OS), Windows_NT)
	del /Q /S $(BUILD)\*
else ifeq ($(shell uname), Linux)
	find $(BUILD) -maxdepth 1 -type f -exec rm {} \;
	rm -rf $(OBJ)/*
endif

mkbuild:
ifeq ($(OS), Windows_NT)
	if not exist "$(BUILD)" mkdir "$(BUILD)"
	if not exist "$(OBJ)" mkdir "$(OBJ)"
else ifeq ($(shell uname), Linux)
	mkdir -p $(BUILD)
	mkdir -p $(OBJ)
endif

install:
ifeq ($(OS), Windows_NT)
else ifeq ($(shell uname), Linux)
	cp benji.service /etc/systemd/system/benjid.service
	cp $(BUILD)/$(EXEC) /usr/local/bin/benjid
endif