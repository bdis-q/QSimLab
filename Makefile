# Makefile for this project on Linux

# ---------------------	macros --------------------------

# compiler and flags
CC := mpic++
CCFLAGS := -std=c++11 -Wall -Werror -pthread

# directories
QSIM_DIR := qsim
DISQ_DIR := disqsim
MAIN_DIR := main
OBJ_DIR := obj

INCFLAGS := -I./$(QSIM_DIR)/ -I./$(DISQ_DIR)/

COMPILE := $(CC) $(CCFLAGS) $(INCFLAGS)

# ---------------- basic simulation ----------------

QSIM_CPPS := $(QSIM_DIR)/*.cpp
QSIM_CPPS := $(wildcard $(QSIM_CPPS))

QSIM_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(QSIM_CPPS)))

# ------------- distributed simulation -------------

DISQ_CPPS := $(DISQ_DIR)/*.cpp
DISQ_CPPS := $(wildcard $(DISQ_CPPS))

DISQ_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(DISQ_CPPS)))

# ---------------------- main ----------------------

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
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(OBJ_DIR)/$(QSIM_DIR)
	@-mkdir -p $(OBJ_DIR)/$(DISQ_DIR)

# utility functions: qsim/*.cpp -> obj/qsim/*.o
$(OBJ_DIR)/%.o: %.cpp
	@echo "[INFO] Compiling" $< ...
	@$(COMPILE) -c $< -o $@

# executable files: main/*.cpp -> obj/*
$(OBJ_DIR)/%: $(OBJ_DIR) $(MAIN_CPPS) $(QSIM_OBJS) $(DISQ_OBJS)
	@echo "[INFO] Linking" $@ ...
	@$(COMPILE) $(patsubst $(OBJ_DIR)/%,$(MAIN_DIR)/%.cpp,$@) $(QSIM_OBJS) $(DISQ_OBJS) -o $@
	@echo "[INFO]" $@ "has been built. "

# clean
clean:
	rm -rf $(OBJ_DIR)

# run with 16 processes
run:
	mpirun -np 16 obj/main