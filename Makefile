CC=gcc
BUILD_DIR=build
OBJ=$(BUILD_DIR)/obj
BIN=$(BUILD_DIR)/bin
IDIR =include
SRC=src
CFLAGS=-c -I$(IDIR)
ifneq ($(BUILD_DIR),)
  $(shell [ -d $(BUILD_DIR) ] || mkdir -p $(BUILD_DIR))
  $(shell [ -d $(OBJ) ] || mkdir -p $(OBJ))
  $(shell [ -d $(BIN) ] || mkdir -p $(BIN))
endif
SRCS:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst %.c,$(OBJ)/%.o,$(notdir $(SRCS)))
ELF:=$(BIN)/app.out
DEPS:=$(OBJS:.o=.d)
.PHONY: all
    all: $(ELF)
$(ELF) : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
$(OBJ)/%.o : $(SRC)/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -fr $(BUILD_DIR) 
