# Makefile for this project
# make && D:\abuqiqi\Project\QMerge\obj\main.exe > output.txt

# ---------------------	macros --------------------------

# compiler and flags
CC := g++
CCFLAGS := -std=c++11 -Wall -Werror

# directories
QSIM_DIR := qsim
MAIN_DIR := main
OBJ_DIR := obj
OUT_DIR := output

INCFLAGS := -I./$(QSIM_DIR)/

COMPILE := $(CC) $(CCFLAGS) $(INCFLAGS)

# ---------------- basic simulation ----------------

QSIM_CPPS := $(QSIM_DIR)/*.cpp
QSIM_CPPS := $(wildcard $(QSIM_CPPS))

QSIM_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(QSIM_CPPS)))

# --------------------- main -----------------------

MAIN_CPPS := $(MAIN_DIR)/test.cpp
MAIN_CPPS := $(wildcard $(MAIN_CPPS))

MAIN_OBJS := $(patsubst $(MAIN_DIR)/%.cpp,$(OBJ_DIR)/%,$(MAIN_CPPS))

# -------------------- targets ---------------------

TARGETS := $(MAIN_OBJS)

.PHONY: all clean
.PRECIOUS: $(OBJ_DIR)/%.o

all: $(TARGETS)

# mkdir obj obj/util output
$(OBJ_DIR):
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)\qsim

# utility functions: util/*.cpp -> obj/util/*.o
$(OBJ_DIR)/%.o: %.cpp
	@echo "[INFO] Compiling" $< ...
	@$(COMPILE) -c $< -o $@

# executable files: main/*.cpp -> obj/*
$(OBJ_DIR)/%: $(OBJ_DIR) $(MAIN_CPPS) $(UTIL_OBJS) $(QSIM_OBJS)
	@echo "[INFO] Linking" $@ ...
	@$(COMPILE) $(patsubst $(OBJ_DIR)/%,$(MAIN_DIR)/%.cpp,$@) $(UTIL_OBJS) $(QSIM_OBJS) -o $@
	@echo "[INFO]" $@ "has been built. "

# clean
clean:
	del /s /q $(OBJ_DIR)