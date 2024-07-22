# Makefile for this project

# ---------------------	macros --------------------------

# compiler and flags
CC := g++
CCFLAGS := -std=c++11 -Wall -Werror

# directories
QSIM_DIR := qsim
MAIN_DIR := main
OBJ_DIR := obj

INCFLAGS := -I./$(QSIM_DIR)/

COMPILE := $(CC) $(CCFLAGS) $(INCFLAGS)

# ---------------- basic simulation ----------------

QSIM_CPPS := $(QSIM_DIR)/*.cpp
QSIM_CPPS := $(wildcard $(QSIM_CPPS))

QSIM_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(QSIM_CPPS)))

# --------------------- main -----------------------

MAIN_CPPS := $(MAIN_DIR)/*.cpp
MAIN_CPPS := $(wildcard $(MAIN_CPPS))

MAIN_OBJS := $(patsubst $(MAIN_DIR)/%.cpp,$(OBJ_DIR)/%,$(MAIN_CPPS))

# -------------------- targets ---------------------

TARGETS := $(MAIN_OBJS)

.PHONY: all clean
.PRECIOUS: $(OBJ_DIR)/%.o

all: $(TARGETS)

# mkdir obj obj/qsim
$(OBJ_DIR):
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)\$(QSIM_DIR)

# utility functions: qsim/*.cpp -> obj/qsim/*.o
$(OBJ_DIR)/%.o: %.cpp
	@echo "[INFO] Compiling" $< ...
	@$(COMPILE) -c $< -o $@

# executable files: main/*.cpp -> obj/*
$(OBJ_DIR)/%: $(OBJ_DIR) $(MAIN_CPPS) $(QSIM_OBJS)
	@echo "[INFO] Linking" $@ ...
	@$(COMPILE) $(patsubst $(OBJ_DIR)/%,$(MAIN_DIR)/%.cpp,$@) $(QSIM_OBJS) -o $@
	@echo "[INFO]" $@ "has been built. "

# clean
clean:
	del /s /q $(OBJ_DIR)
