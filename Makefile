CC = gcc

INCLUDE_DIR=include_compilation
GLFW_INCLUDE=$(INCLUDE_DIR)/glfw
SOIL_INCLUDE=$(INCLUDE_DIR)/soil
GAMEDEV_DIR=Y:/gamedev

LINUX_FLAGS = -lGLEW -lX11 -lXrandr -lXxf86vm -lXinerama -lXcursor -lasound -lportaudio -lXi -ldl -lrt -lGL

WINDOWS_FLAGS = -lgdi32 -lopengl32 #-Wl,-subsystem,windows
WINDOWS_INCLUDES = -I$(GLFW_INCLUDE)/include -I$(GLFW_INCLUDE)/deps -I$(GLFW_INCLUDE)/src
WINDOWS_LIB = $(INCLUDE_DIR)/libportaudio.dll.a $(INCLUDE_DIR)/libglew32.a $(INCLUDE_DIR)/libglew32.dll.a \
	      $(INCLUDE_DIR)/win/libSOIL.a

COMMON_FLAGS = -lglfw3 -lpthread -lm -lpthread
COMMON_INCLUDES = -I$(INCLUDE_DIR)

ERROR_FLAGS = -Wall -Wextra -pedantic -std=c99 -Wwrite-strings

CFLAGS = $(COMMON_FLAGS)
LD = $(CC)
LDFLAGS = -I$(INCLUDE_DIR) $(ERROR_FLAGS)

SOURCE_DIR=source
SHADERS = $(SOURCE_DIR)/shaders
TEXTURES = textures

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MAKEFILE_PATH))))
CURRENT_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

FILE_SUFFIX =

ifeq ($(OS),Windows_NT)
OBJECT_DIR = obj/win
EXECUTABLES_DIR = executables/win

CFLAGS += $(COMMON_INCLUDES)
CFLAGS += $(WINDOWS_INCLUDES)
CFLAGS += $(WINDOWS_LIB)
CFLAGS += $(WINDOWS_FLAGS)
LDFLAGS += $(WINDOWS_INCLUDES)
LDFLAGS += $(WINDOWS_LIB)

FILE_SUFFIX = .exe
else
OBJECT_DIR = obj/linux
EXECUTABLES_DIR = executables/linux
CFLAGS += $(LINUX_FLAGS)
CFLAGS += $(COMMON_INCLUDES)
LDFLAGS += $(CFLAGS)
COMMON_LIBS = $(INCLUDE_DIR)/linux/libSOIL.a
endif

CFILES = $(wildcard $(SOURCE_DIR)/*.c)
TARGETS = $(patsubst %.c, %, $(CFILES:$(SOURCE_DIR)/%=%))
OBJECTS = $(patsubst %, $(OBJECT_DIR)/%.o, $(TARGETS))
_DEPENDENCIES = $(wildcard $(INCLUDE_DIR)/*.c)
DEPENDENCIES = $(patsubst %.c, $(OBJECT_DIR)/%.o, $(_DEPENDENCIES:$(INCLUDE_DIR)/%=%))

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(OBJECT_DIR)
	$(CC) -c -o $@ $< $(LDFLAGS)

$(OBJECT_DIR)/%.o: $(INCLUDE_DIR)/%.c
	@mkdir -p $(OBJECT_DIR)
	$(CC) -c -o $@ $< $(LDFLAGS)

all: $(TARGETS)
	cp -r $(SHADERS) $(EXECUTABLES_DIR)
ifeq ($(OS), Windows_NT)
	unix2dos $(EXECUTABLES_DIR)/shaders/*
	cp -r $(TEXTURES) $(EXECUTABLES_DIR)
	cp -r $(EXECUTABLES_DIR)/* $(GAMEDEV_DIR)
else
	ln -sf $(CURRENT_PATH)$(TEXTURES) $(CURRENT_PATH)$(EXECUTABLES_DIR)
endif

.SECONDEXPANSION:
$(TARGETS): $(DEPENDENCIES) $(OBJECT_DIR)/$$@.o
	@$(CC) -o $(EXECUTABLES_DIR)/$@$(FILE_SUFFIX) $^ $(ERROR_FLAGS) $(CFLAGS) $(COMMON_LIBS)
	@echo "$@$(FILE_SUFFIX)"

.PHONY: clean
clean:
	rm -rf $(OBJECT_DIR)/*.o
	rm -rf $(EXECUTABLES_DIR)/*
