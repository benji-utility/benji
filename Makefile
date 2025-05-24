GXX = gcc

GXX_FLAGS = -g -Wno-discarded-qualifiers

MAIN_SRC = src
INSTALLER_SRC = installer
TOML = $(MAIN_SRC)/include/toml-c

BUILD = build
OBJ = $(BUILD)/obj

MAIN_EXEC =
INSTALL_EXEC = benji-installer

ifeq ($(OS), Windows_NT)
	MAIN_EXEC = benji-service
else ifeq ($(shell uname), Linux)
	MAIN_EXEC = benjid
endif

MAIN_SRCS = $(wildcard $(MAIN_SRC)/*.c)
INSTALLER_SRCS = $(wildcard $(INSTALLER_SRC)/*.c)
TOML_SRCS = $(TOML)/toml.c

CONFIG_FILE =
TEST_DATA =
TEST_PORT =

ifeq ($(OS), Windows_NT)
	LINKED_LIBS = -lWs2_32 -ldxgi -ldxguid -lole32
else ifeq ($(shell uname), Linux)
	LINKED_LIBS =
endif

INCLUDES = -I$(TOML)

OBJS = $(patsubst $(MAIN_SRC)/%.c, $(OBJ)/%.o, $(MAIN_SRCS))
OBJS += $(patsubst $(TOML)/%.c, $(OBJ)/%.o, $(TOML_SRCS))

all: clean compile

compile: $(BUILD)/$(MAIN_EXEC)

$(BUILD)/$(MAIN_EXEC): $(OBJS)
	$(GXX) $(OBJS) -o $@ $(LINKED_LIBS)

$(OBJ)/%.o: $(MAIN_SRC)/%.c
	$(GXX) $(GXX_FLAGS) $(INCLUDES) -c $< -o $@

$(OBJ)/%.o: $(TOML)/%.c
	$(GXX) $(GXX_FLAGS) $(INCLUDES) -c $< -o $@

ifeq ($(OS), Windows_NT)
.SILENT: clean test
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

build_installer: all
ifeq ($(OS), Windows_NT)
	if exist "$(BUILD)/$(INSTALL_EXEC).exe" del /S $(BUILD)\$(INSTALL_EXEC).exe
	$(GXX) $(GXX_FLAGS) $(INSTALLER_SRCS) -o $(BUILD)/$(INSTALL_EXEC)
else ifeq ($(shell uname), Linux)
	cp benji.service /etc/systemd/system/benjid.service
	cp $(BUILD)/$(MAIN_EXEC) /usr/local/bin/benjid
endif

test:
ifeq ($(OS), Windows_NT)
	(set /p="$(TEST_DATA)") <nul | ncat 127.0.0.1 $(TEST_PORT)
endif