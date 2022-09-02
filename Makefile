CCVERSION := x86_64-w64-mingw32
CC := $(CCVERSION)-gcc# FILL: the compiler
CXX := $(CCVERSION)-g++# FILL: the compiler
#-L/usr/$(CCVERSION)/sys-root/mingw/lib/
#-I/usr/$(CCVERSION)/sys-root/mingw/include/
INCDIR := -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/freetype2/ -Iinc
LIBS := -lmingw32 -lcairo -lfreetype -lharfbuzz -lharfbuzz-icu -lSDL2main -lSDL2 -mwindows
FLAGS := --std=c99 -Wall -Wpedantic -Werror=implicit-function-declaration -fstack-clash-protection -O2
CFLAGS := $(INCDIR) $(LIBS) $(FLAGS)
CXXFLAGS := $(INCDIR) $(LIBS) $(FLAGS)
DBGFLAGS := -g
COBJFLAGS := $(CFLAGS) -c

# path macros
SRC_PATH := src
BIN_PATH := build/bin
OBJ_PATH := build/obj
DBG_PATH := build/debug

# compile macros
TARGET_NAME := sdl2_cairo_clock# FILL: target name
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets
$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(COBJFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(COBJFLAGS) $(DBGFLAGS) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) -o $@ $(OBJ_DEBUG) $(CFLAGS) $(DBGFLAGS)  

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)