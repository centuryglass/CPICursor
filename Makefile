define HELPTEXT
## CPICursor Makefile ##
# Usage: make [target] [options]
# Typical installation process:
#   make
#   make install
endef
export HELPTEXT

####################### Initialize build variables: ##########################
TARGET_APP=CPICursor
# Version number:
APP_VERSION=0.0.1
# Version hex.
APP_VERSION_HEX=0x1
# Build type: either Debug or Release
CONFIG?=Release
# Command used to strip unneeded symbols from object files:
STRIP?=strip
# Use the build system's architecture by default.
TARGET_ARCH?=-march=native
# Enable or disable verbose output
VERBOSE?=0
V_AT:=$(shell if [ $(VERBOSE) != 1 ]; then echo '@'; fi)

.PHONY: build clean install uninstall \
        painterd-build painterd-clean painterd-install painterd-uninstall \
        keyd-build keyd-clean keyd-install keyd-uninstall

########################### Project directories: #############################
PROJECT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SOURCE_DIR:=$(PROJECT_DIR)/Source
BUILD_DIR:=$(PROJECT_DIR)/build/$(CONFIG)
OBJDIR:=$(BUILD_DIR)/intermediate
INSTALL_DIR=/usr/bin
TMP_DIR=/var/tmp/.$(TARGET_APP)

DATA_PATH:=/usr/share/$(TARGET_APP)
TARGET_BUILD_PATH:=$(BUILD_DIR)/$(TARGET_APP)
TARGET_INSTALL_PATH:=$(INSTALL_DIR)/$(TARGET_APP)

# Dependency directories:
DAEMON_FRAMEWORK_DIR:=$(PROJECT_DIR)/deps/DaemonFramework
KEY_DAEMON_DIR:=$(PROJECT_DIR)/deps/KeyDaemon
FBPAINTER_DIR:=$(PROJECT_DIR)/deps/FBPainter

########################## Primary build target: ##############################
$(TARGET_BUILD_PATH) : build
	@echo Linking "$(TARGET_APP):"
	@if [ "$(VERBOSE)" == "1" ]; then \
        $(DAEMON_FRAMEWORK_DIR)/cleanPrint.sh '$(CXX) $(LINK_ARGS)'; \
        echo ''; \
    fi
	@$(CXX) $(LINK_ARGS)

########################## Daemon parent support: ############################
DF_OBJDIR:=$(OBJDIR)/DaemonFramework
DF_CONFIG:=$(CONFIG)
DF_VERBOSE:=$(VERBOSE)

include $(DAEMON_FRAMEWORK_DIR)/Parent.mk

######################## Cursor KeyDaemon building: ##########################
KEY_DAEMON=cursorKeyd
KEYD_DIR:=$(PROJECT_DIR)/$(KEY_DAEMON)
KEYD_BUILD:=$(KEYD_DIR)/build/$(CONFIG)
KEYD_PATH:=$(KEYD_BUILD)/$(KEY_DAEMON)
KEYD_PIPE_FILE=.keyPipe
KEYD_LOCK_FILE=.keyLock

# Definitions needed for building KeyDaemon control class:
KD_OBJDIR:=$(OBJDIR)/DaemonFramework
KD_PARENT_PATH:=$(TARGET_INSTALL_PATH)
KD_DAEMON_PATH:=$(DATA_PATH)/$(KEY_DAEMON)
KD_PIPE_PATH:=$(TMP_DIR)/$(KEYD_PIPE_FILE)
KD_KEY_LIMIT:=7  # Four arrow keys, left-click, right-click, cancel
KD_CONFIG:=$(CONFIG)
KD_VERBOSE?=0

include $(KEY_DAEMON_DIR)/Parent.mk

KEYD_MAKE:=make -f $(KEYD_DIR)/Makefile

KEYD_MAKEARGS:=KD_TARGET_APP=$(KEY_DAEMON) KD_INSTALL_DIR=$(DATA_PATH) \
              KD_BUILD_DIR=$(KEYD_BUILD) \
              DAEMON_FRAMEWORK_DIR=$(DAEMON_FRAMEWORK_DIR) \
              KD_KEY_LIMIT=$(KD_KEY_LIMIT) \
              KD_PARENT_PATH=$(TARGET_INSTALL_PATH) \
              KD_PIPE_PATH=$(TMP_DIR)/$(KEYD_PIPE_FILE) \
              KD_LOCK_PATH=$(TMP_DIR)/$(KEYD_LOCK_FILE) \
              KD_CONFIG:=$(CONFIG) \
              KD_VERBOSE:=$(VERBOSE)

keyd-build :
	@echo "building $(KEY_DAEMON)"
	-$(V_AT)$(KEYD_MAKE) $(KEYD_MAKEARGS)

keyd-clean :
	-$(V_AT)$(KEYD_MAKE) clean $(KEYD_MAKEARGS)

keyd-install : keyd-build
	-$(V_AT)$(KEYD_MAKE) install $(KEYD_MAKEARGS)

keyd-uninstall :
	-$(V_AT)$(KEYD_MAKE) uninstall $(KEYD_MAKEARGS)

##################### Cursor Painter Daemon building: ########################
PAINTER_DAEMON=cursorPainterd
PAINTERD_DIR:=$(PROJECT_DIR)/$(PAINTER_DAEMON)
PAINTERD_BUILD_DIR:=$(PAINTERD_DIR)/build/$(CONFIG)
PAINTERD_BUILD_PATH:=$(PAINTERD_BUILD_DIR)/$(PAINTER_DAEMON)
PAINTERD_PATH:=$(DATA_PATH)/$(PAINTER_DAEMON)
PAINTERD_INPUT_PIPE_FILE=.paintIn
PAINTERD_OUTPUT_PIPE_FILE=.paintOut
PAINTERD_LOCK_FILE=.paintLock
PAINTERD_INPUT_PIPE_PATH:=$(DATA_PATH)/$(PAINTERD_INPUT_PIPE_FILE)
PAINTERD_OUTPUT_PIPE_PATH:=$(DATA_PATH)/$(PAINTERD_OUTPUT_PIPE_FILE)
PAINTERD_LOCK_PATH:=$(TMP_DIR)/$(PAINTERD_LOCK_FILE)

PAINTERD_MAKE:=make -f $(PAINTERD_DIR)/Makefile

PAINTERD_MAKEARGS:=TARGET_APP=$(PAINTER_DAEMON) \
                   BUILD_DIR=$(PAINTERD_BUILD_DIR) \
                   INSTALL_DIR=$(DATA_PATH) \
                   PARENT_PATH=$(INSTALL_PATH) \
                   INPUT_PIPE_PATH=$(PAINTERD_INPUT_PIPE_PATH) \
                   OUTPUT_PIPE_PATH=$(PAINTERD_OUTPUT_PIPE_PATH) \
                   LOCK_PATH=$(PAINTERD_LOCK_PATH) \
                   CONFIG=$(CONFIG) \
                   VERBOSE=$(VERBOSE)

painterd-build :
	@echo "building $(PAINTER_DAEMON)"
	-$(V_AT)$(PAINTERD_MAKE) $(PAINTERD_BUILD_PATH) $(PAINTERD_MAKEARGS)

painterd-clean :
	-$(V_AT)$(PAINTERD_MAKE) clean $(PAINTERD_MAKEARGS)

painterd-install :
	-$(V_AT)$(PAINTERD_MAKE) install $(PAINTERD_MAKEARGS)

painterd-uninstall :
	-$(V_AT)$(PAINTERD_MAKE) uninstall $(PAINTERD_MAKEARGS)

############################## Set build flags: ##############################
#### Config-specific flags: ####
ifeq ($(CONFIG),Debug)
    OPTIMIZATION?=0
    GDB_SUPPORT?=1
    # Debug-specific preprocessor definitions:
    CONFIG_FLAGS=-DDEBUG=1
endif

ifeq ($(CONFIG),Release)
    OPTIMIZATION?=1
    GDB_SUPPORT?=0
endif

# Set optimization level flags:
ifeq ($(OPTIMIZATION),1)
    CONFIG_CFLAGS=-O3 -flto
    CONFIG_LDFLAGS:=-flto
else
    CONFIG_CFLAGS=-O0
endif

# Set debugger flags:
ifeq ($(GDB_SUPPORT),1)
    CONFIG_CFLAGS:=$(CONFIG_CFLAGS) -g -ggdb
else
    CONFIG_LDFLAGS:=$(CONFIG_LDFLAGS) -fvisibility=hidden
endif

#### C compilation flags: ####
CFLAGS:=$(TARGET_ARCH) $(CONFIG_CFLAGS) $(CFLAGS)

#### C++ compilation flags: ####
CXXFLAGS:=-std=gnu++14 $(CXXFLAGS)

#### C Preprocessor flags: ####

# Include directories:
INCLUDE_FLAGS:=-I$(SOURCE_DIR) \
               $(DF_INCLUDE_FLAGS) \
               $(KD_INCLUDE_FLAGS) \
               $(INCLUDE_FLAGS)

# Disable dependency generation if multiple architectures are set
DEPFLAGS:=$(if $(word 2, $(TARGET_ARCH)), , -MMD)

DEFINE_FLAGS:=$(call addStringDef,PAINTERD_PATH) \
              $(call addStringDef,PAINTERD_INPUT_PIPE_PATH) \
              $(call addStringDef,PAINTERD_OUTPUT_PIPE_PATH) \
              $(DF_DEFINE_FLAGS) \
              $(KD_DEFINE_FLAGS) \
              $(FBP_DEFINE_FLAGS) $(DEFINE_FLAGS)

CPPFLAGS:=-pthread \
          $(DEPFLAGS) \
          $(CONFIG_FLAGS) \
          $(DEFINE_FLAGS) \
          $(INCLUDE_FLAGS) \
          $(CPPFLAGS)

#### Linker flags: ####
LDFLAGS := -lpthread $(TARGET_ARCH) $(CONFIG_LDFLAGS) $(LDFLAGS)

#### Aggregated build arguments: ####

OBJECTS:=$(OBJDIR)/Main.o \
         $(OBJDIR)/CursorPainter.o \
         $(OBJDIR)/DisplayListener.o \
         $(OBJDIR)/KeyListener.o

# Complete set of flags used to compile source files:
BUILD_FLAGS:=$(CFLAGS) $(CXXFLAGS) $(CPPFLAGS)

# Complete set of arguments used to link the program:
LINK_ARGS:= -o $(TARGET_BUILD_PATH) $(OBJECTS) $(DF_OBJECTS_PARENT) \
               $(KD_OBJECTS_PARENT) $(LDFLAGS)

###################### Supporting Build Targets: ##############################

build : df-parent kd-parent keyd-build painterd-build $(OBJECTS)

clean : keyd-clean painterd-clean
	@echo "Cleaning $(TARGET_APP)"
	$(V_AT)if [ -d $(OBJDIR) ]; then \
	    rm -rf $(OBJDIR); \
    fi; \
    if [ -f $(TARGET_BUILD_PATH) ]; then \
	    rm $(TARGET_BUILD_PATH); \
    fi

install : keyd-install painterd-install
	@echo "Installing $(TARGET_APP)"
	-$(V_AT)sudo cp $(TARGET_BUILD_PATH) $(TARGET_INSTALL_PATH)
	-$(V_AT)if [ ! -d $(DATA_PATH) ]; then \
		sudo mkdir $(DATA_PATH) ; \
	fi

uninstall : keyd-uninstall painterd-uninstall
	@echo "Uninstalling $(TARGET_APP)"
	-$(V_AT)sudo rm $(TARGET_INSTALL_PATH) && sudo rm -r  $(DATA_PATH)

$(OBJECTS) :
	@echo "Compiling $(<F):"
	$(V_AT)mkdir -p $(OBJDIR)
	@if [ "$(VERBOSE)" == "1" ]; then \
        $(DAEMON_FRAMEWORK_DIR)/cleanPrint.sh '$(CXX) $(BUILD_FLAGS)'; \
        echo '    -o "$@" \'; \
        echo '    -c "$<"'; \
        echo ''; \
	fi
	$(CXX) $(BUILD_FLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)

$(OBJDIR)/Main.o: \
    $(SOURCE_DIR)/Main.cpp
$(OBJDIR)/CursorPainter.o: \
    $(SOURCE_DIR)/CursorPainter.cpp
$(OBJDIR)/DisplayListener.o: \
    $(SOURCE_DIR)/DisplayListener.cpp
$(OBJDIR)/KeyListener.o: \
    $(SOURCE_DIR)/KeyListener.cpp
