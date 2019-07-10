define HELPTEXT
## CPICursor Makefile ##
# Usage: make [target] [options]
# Typical installation process:
#   make
#   make install
endef
export HELPTEXT

######### Initialize build variables: #########
# Executable name:
TARGET_APP = CPICursor
# Version number:
APP_VERSION = 0.0.1
# Version hex.
APP_VERSION_HEX = 0x1

# Default Build Options:
# Build type: either Debug or Release
CONFIG ?= Release
# Command used to strip unneeded symbols from object files:
STRIP ?= strip
# Use the build system's architecture by default.
TARGET_ARCH ?= -march=native
# Enable or disable verbose output
V ?= 0
# Enable or disable libpng support
USE_LIBPNG ?= 1

# Build directories:
BINDIR := build
LIBDIR := build
# Directory where .o files will be created:
OBJDIR := build/intermediate
# Directory where executable files will be created:
OUTDIR := build
# Data installation directory:
DATA_PATH := /usr/share/$(TARGET_APP)

PKG_CONFIG_LIBS =

# List additional library flags here:
LDFLAGS := $(LDFLAGS)

# Extra compilation flags:
CPPFLAGS := $(CPPFLAGS)

# Extra compilation flags (C++ only):
CXXFLAGS := -std=gnu++14 $(CXXFLAGS)

# Directories to search for header files:
INCLUDE_DIRS :=

# Directories to recursively search for header files:
RECURSIVE_INCLUDE_DIRS := Source

#### Setup: ####

# build with "V=1" for verbose builds
ifeq ($(V), 1)
    V_AT =
else
    V_AT = @
endif

# Disable dependency generation if multiple architectures are set
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

# Generate the list of directory include flags:
DIR_FLAGS := $(shell echo $(INCLUDE_DIRS) | xargs printf " -I'%s'") \
             $(shell find $(RECURSIVE_INCLUDE_DIRS) -type d \
                     -printf " -I'%p'")

# Keep debug and release build files in separate directories:
OBJDIR := $(OBJDIR)/$(CONFIG)
OUTDIR := $(OUTDIR)/$(CONFIG)

ifeq ($(CONFIG),Debug)
    # Disable optimization and enable gdb flags and tests unless otherwise
    # specified:
    OPTIMIZATION ?= 0
    GDB_SUPPORT ?= 1
    BUILD_TESTS ?= 1
    # Debug-specific preprocessor definitions:
    CONFIG_FLAGS = -DDEBUG=1 -D_DEBUG=1
    ifeq ($(V), 1)
        CONFIG_FLAGS := -DDEBUG_VERBOSE=1 $(CONFIG_FLAGS)
    endif
endif

ifeq ($(CONFIG),Release)
    # Enable optimization and disable gdb flags and tests unless otherwise
    # specified:
    OPTIMIZATION ?= 1
    GDB_SUPPORT ?= 0
    BUILD_TESTS ?= 0
    # Release-specific preprocessor definitions:
    CONFIG_FLAGS = -DNDEBUG=1
endif

# Set optimization level flags:
ifeq ($(OPTIMIZATION), 1)
    CONFIG_CFLAGS := $(CONFIG_CFLAGS) -O3 -flto
    CONFIG_LDFLAGS := $(CONFIG_LDFLAGS) -flto
else
    CONFIG_CFLAGS := $(CONFIG_CFLAGS) -O0
endif

# Set debugging flags:
ifeq ($(GDB_SUPPORT), 1)
    CONFIG_CFLAGS := $(CONFIG_CFLAGS) -g -ggdb
else
    CONFIG_LDFLAGS := $(CONFIG_LDFLAGS) -fvisibility=hidden
endif

CPPFLAGS := $(DEPFLAGS) \
            $(CONFIG_FLAGS) \
	        $(DIR_FLAGS) \
            $(shell pkg-config --cflags $(PKG_CONFIG_LIBS)) \
            $(CPPFLAGS)

CFLAGS := $(CPPFLAGS) \
		  $(TARGET_ARCH) \
          $(CONFIG_CFLAGS) \
          $(CFLAGS)

CXXFLAGS := $(CFLAGS) \
            $(CXXFLAGS)

LDFLAGS := $(TARGET_ARCH) \
	       -L$(BINDIR) \
	       -L$(LIBDIR) \
	        $(CONFIG_LDFLAGS) \
            $(shell pkg-config --libs $(PKG_CONFIG_LIBS)) \
	        $(LDFLAGS)

CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)

.PHONY: build install debug release clean strip uninstall help
build : $(OUTDIR)/$(TARGET_APP)

OBJECTS_APP := $(OBJDIR)/Main.o \
               $(OBJECTS_APP)

$(OUTDIR)/$(TARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@echo Linking "$(TARGET_APP)"
	-$(V_AT)mkdir -p $(BINDIR)
	-$(V_AT)mkdir -p $(LIBDIR)
	-$(V_AT)mkdir -p $(OUTDIR)
	$(V_AT)$(CXX) -o $(OUTDIR)/$(TARGET_APP) $(OBJECTS_APP) \
		             $(LDFLAGS) $(LDFLAGS_APP) $(RESOURCES) \
					 $(TARGET_ARCH)

$(OBJECTS_APP) :
	-$(V_AT)mkdir -p $(OBJDIR)
	@echo "      Compiling: $(<F)"
	$(V_AT)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(CFLAGS) \
		-o "$@" -c "$<"

install:
	killall $(TARGET_APP);\
	sudo cp build/$(CONFIG)/$(TARGET_APP) $(INSTALL_PATH); \
	if [ ! -d $(DATA_PATH) ; \
        sudo mkdir $(DATA_PATH) ; \
    fi && \
    sudo cp -R assets/* $(DATA_PATH)


debug:
	$(MAKE) CONFIG=Debug
	reset
	$(MAKE) install CONFIG=Debug
	gdb $(TARGET_APP)

release:
	$(MAKE) CONFIG=Release
	reset
	$(MAKE) install CONFIG=Release

clean:
	@echo Cleaning $(TARGET_APP)
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping $(TARGET_APP)
	-$(V_AT)$(STRIP) --strip-unneeded $(OUTDIR)/$(TARGET)

uninstall:
	@echo Uninstalling $(TARGET_APP)
	killall $(TARGET_APP);\
	sudo rm $(INSTALL_PATH) ; \

help:
	@echo "$$HELPTEXT"

-include $(OBJECTS_APP:%.o=%.d)

$(OBJDIR)/Main.o: \
	Main.cpp
