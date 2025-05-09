GXX = gcc

GXX_FLAGS = -g -Wno-discarded-qualifiers

SRC = src
TOML = $(SRC)/include/toml-c

BUILD = build
OBJ = $(BUILD)/obj

EXEC = benji

SRCS = $(wildcard src/*.c)
TOML_SRCS = $(wildcard $(TOML)/toml.c)

CONFIG_FILE =
TEST_DATA =
TEST_PORT =

ifeq ($(OS), Windows_NT)
	LINKED_LIBS = -lWs2_32 -ldxgi -ldxguid -lole32
else ifeq ($(shell uname), Linux)
	LINKED_LIBS =
endif

INCLUDES = -I$(TOML)
DEFINES = -DBENJI_CONFIG_PATH=$(abspath $(CONFIG_FILE))

OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
OBJS += $(patsubst $(TOML)/%.c, $(OBJ)/%.o, $(TOML_SRCS))

all: clean compile

compile: $(BUILD)/$(EXEC)

$(BUILD)/$(EXEC): $(OBJS)
	$(GXX) $(OBJS) -o $@ $(LINKED_LIBS)

$(OBJ)/%.o: $(SRC)/%.c
	$(GXX) $(GXX_FLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ)/%.o: $(TOML)/%.c
	$(GXX) $(GXX_FLAGS) $(INCLUDES) -c $< -o $@

ifeq ($(OS), Windows_NT)
.SILENT: install clean test
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
	echo Not supported on Windows (yet)
else ifeq ($(shell uname), Linux)
	cp benji.service /etc/systemd/system/benjid.service
	cp $(BUILD)/$(EXEC) /usr/local/bin/benjid
endif

test:
ifeq ($(OS), Windows_NT)
	(set /p="$(TEST_DATA)") <nul | ncat 127.0.0.1 $(TEST_PORT)
endif