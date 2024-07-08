# Makefile for this project
# make && D:\abuqiqi\Project\QMerge\obj\main.exe > output.txt

# ---------------------	macros --------------------------

# compiler and flags
CC := g++
CCFLAGS := -std=c++11 -Wall -Werror

# directories
QSIM_DIR := qsim
UTIL_DIR := util
MAIN_DIR := main
OBJ_DIR := obj
OUT_DIR := output

INCFLAGS := -I./$(QSIM_DIR)/ -I./$(UTIL_DIR)/

COMPILE := $(CC) $(CCFLAGS) $(INCFLAGS)

# ---------------- basic simulation ----------------

QSIM_CPPS := $(QSIM_DIR)/*.cpp
QSIM_CPPS := $(wildcard $(QSIM_CPPS))

QSIM_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(QSIM_CPPS)))

# --------------------- utils ----------------------

UTIL_CPPS := $(UTIL_DIR)/*.cpp
UTIL_CPPS := $(wildcard $(UTIL_CPPS))

UTIL_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(UTIL_CPPS)))

# --------------------- main -----------------------

MAIN_CPPS := $(MAIN_DIR)/main.cpp
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
	mkdir $(OBJ_DIR)\util
	mkdir $(OBJ_DIR)\qsim
# @-mkdir -p $(OBJ_DIR)
# @-mkdir -p $(dir $(UTIL_OBJS))
# @-mkdir -p $(OUT_DIR)

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
# rm -rf $(OBJ_DIR)
	del /s /q $(OBJ_DIR)

cleanssd:
	del /s /q $(OUT_DIR)