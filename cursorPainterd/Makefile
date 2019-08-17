# cursorPainterd is responsible for drawing the cursor to the frame buffer.
# To access the frame buffer, it always executes as the "video" group.
#
#  To keep its extra privileges from being exploited, it uses DaemonFramework
# to ensure that it may only be controlled by CPICursor, and it restricts
# itself to drawing a single cursor image embedded in its source code.
#
## Quick Guide: ##
# 1. Ensure the following variables are defined when building the daemon:
#    - TARGET_APP
#    - BUILD_DIR
#    - INSTALL_DIR
#    - PARENT_PATH
#    - INPUT_PIPE_PATH
#    - OUTPUT_PIPE_PATH
#    - LOCK_PATH
#
# 2. Optionally, provide valid definitions for these additional variables to
#    enable features or override default values:
#    - CONFIG
#    - VERBOSE
###

######################## Initialize build variables: ##########################
# Default Build Options:
# Build type: either Debug or Release
CONFIG?=Release
# Command used to strip unneeded symbols from object files:
STRIP?=strip
# Use the build system's architecture by default.
TARGET_ARCH?=-march=native
# Enable or disable verbose output
VERBOSE?=0
V_AT:=$(shell if [ $(VERBOSE) != 1 ]; then echo '@'; fi)
# Name of the group given special access to the frame buffer:
FB_GROUP=video
# Path to the frame buffer device file:
FB_PATH=/dev/fb0

# Define project directories:
PAINTERD_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SOURCE_DIR:=$(PAINTERD_DIR)/Source
PROJECT_DIR:=$(shell dirname $(PAINTERD_DIR))
DAEMON_FRAMEWORK_DIR:=$(PROJECT_DIR)/deps/DaemonFramework
FBPAINTER_DIR:=$(PROJECT_DIR)/deps/FBPainter
OBJDIR:=$(PAINTERD_DIR)/build/$(CONFIG)

# Define specific file paths:
TARGET_BUILD_PATH:=$(BUILD_DIR)/$(TARGET_APP)

# Set default build target:
.PHONY: build clean install uninstall
build : $(TARGET_BUILD_PATH)

####################### Daemon Framework setup: ##############################
DF_OBJDIR:=$(OBJDIR)/DaemonFramework
DF_CONFIG:=$(CONFIG)
DF_VERBOSE:=$(VERBOSE)
DF_INPUT_PIPE_PATH:=$(INPUT_PIPE_PATH)
DF_OUTPUT_PIPE_PATH:=$(OUTPUT_PIPE_PATH)
DF_DAEMON_PATH:=$(INSTALL_DIR)/$(TARGET_APP)
DF_REQUIRED_PARENT_PATH:=$(PARENT_PATH)
DF_LOCK_FILE_PATH:=$(LOCK_PATH)

include $(DAEMON_FRAMEWORK_DIR)/Daemon.mk

########################### FBPainter setup: #################################
FBP_OBJDIR:=$(OBJDIR)/FBPainter
FBP_ENABLE_LIBPNG=0
FBP_CONFIG:=$(CONFIG)
FBP_VERBOSE:=$(VERBOSE)

include $(FBPAINTER_DIR)/Makefile

# Build image encoder:
IMG_ENCODE_DIR:=$(FBPAINTER_DIR)/ImageEncoder
FBP_ENCODER:=$(IMG_ENCODE_DIR)/ImageEncoder
$(FBP_ENCODER) :
	@echo "Building ImageEncoder"
	$(V_AT)make -f $(IMG_ENCODE_DIR)/Makefile

# Generate cursor code files:
CURSOR_NAME=Cursor
CURSOR_IMAGE:=$(PAINTERD_DIR)/$(CURSOR_NAME).png
CURSOR_CPP:=$(SOURCE_DIR)/$(CURSOR_NAME).cpp
CURSOR_H:=$(SOURCE_DIR)/$(CURSOR_NAME).h
$(CURSOR_CPP) : $(FBP_ENCODER)
	@echo "Encoding $(CURSOR_NAME).png"
	$(V_AT)$(FBP_ENCODER) $(CURSOR_IMAGE)
	$(V_AT)mv $(PAINTERD_DIR)/$(CURSOR_NAME).h $(CURSOR_H)
	$(V_AT)mv $(PAINTERD_DIR)/$(CURSOR_NAME).cpp $(CURSOR_CPP)

# Create the input pipe file, allowing only the file owner to read and write
# the file.
# TODO: Make the daemon run with setuid as a fake (unprivileged) user that does
#       not own this pipe, so group permissions will apply to the daemon.
#       Once that's done, take read permissions away from the installing user.
INSTALLING_USER:=$$USER
$(INPUT_PIPE_PATH) :
	@echo "Creating input pipe file at \"$(INPUT_PIPE_PATH)\""
	$(V_AT)if [ -f $(INPUT_PIPE_PATH) ]; then \
	    sudo rm $(INPUT_PIPE_PATH); \
	fi
	$(V_AT)sudo mkfifo $(INPUT_PIPE_PATH)
	$(V_AT)sudo chown $(INSTALLING_USER):$(FB_GROUP) $(INPUT_PIPE_PATH)
	$(V_AT)sudo chmod 'u=wr,g=r,o=' $(INPUT_PIPE_PATH)

$(OUTPUT_PIPE_PATH) :
	@echo "Creating output pipe file at \"$(OUTPUT_PIPE_PATH)\""
	$(V_AT)if [ -f $(OUTPUT_PIPE_PATH) ]; then \
	    sudo rm $(OUTPUT_PIPE_PATH); \
	fi
	$(V_AT)sudo mkfifo $(OUTPUT_PIPE_PATH)
	$(V_AT)sudo chown $(INSTALLING_USER):$(FB_GROUP) $(OUTPUT_PIPE_PATH)
	$(V_AT)sudo chmod 'u=wr,g=w,o=' $(OUTPUT_PIPE_PATH)

############################## Set build flags: ##############################
#### Config-specific flags: ####
ifeq ($(CONFIG),Debug)
    OPTIMIZATION?=0
    GDB_SUPPORT?=1
    # Debug-specific preprocessor definitions:
    CONFIG_FLAGS=-DDEBUG=1 -DDF_DEBUG=1
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
INCLUDE_FLAGS:=-I$(SOURCE_DIR) $(FBP_INCLUDE_FLAGS) $(DF_INCLUDE_FLAGS) \
               $(INCLUDE_FLAGS)

# Disable dependency generation if multiple architectures are set
DEPFLAGS:=$(if $(word 2, $(TARGET_ARCH)), , -MMD)

DEFINE_FLAGS:=$(call addStringDef,FB_PATH) $(DF_DEFINE_FLAGS) \
              $(FBP_DEFINE_FLAGS) $(DEFINE_FLAGS)

CPPFLAGS:=-pthread \
          $(DEPFLAGS) \
          $(CONFIG_FLAGS) \
          $(DEFINE_FLAGS) \
          $(INCLUDE_FLAGS) \
          $(CPPFLAGS)

#### Linker flags: ####
LDFLAGS:=-lpthread $(KD_TARGET_ARCH) $(CONFIG_LDFLAGS) $(LDFLAGS)

#### Aggregated build arguments: ####

PAINTERD_OBJECTS:=$(OBJDIR)/Main.o \
                  $(OBJDIR)/Cursor.o \
                  $(OBJDIR)/PainterLoop.o
 
# Complete set of flags used to compile source files:
BUILD_FLAGS:=$(CFLAGS) $(CXXFLAGS) $(CPPFLAGS)

# Complete set of arguments used to link the program:
LINK_ARGS:= -o $(TARGET_BUILD_PATH) $(PAINTERD_OBJECTS) $(DF_OBJECTS_DAEMON) \
            $(FBPAINTER_OBJECTS) $(LDFLAGS)

########################## Primary build target: ##############################
$(TARGET_BUILD_PATH) : fbpainter df-daemon $(PAINTERD_OBJECTS)
	@echo Linking "$(TARGET_APP):"
	@if [ "$(VERBOSE)" == "1" ]; then \
        $(DAEMON_FRAMEWORK_DIR)/cleanPrint.sh '$(CXX) $(LINK_ARGS)'; \
        echo ''; \
    fi
	@$(CXX) $(LINK_ARGS)

###################### Supporting Build Targets: ##############################

clean:
	@echo "Cleaning $(TARGET_APP):"
	$(V_AT)if [ -d $(OBJDIR) ]; then \
	    rm -rf $(OBJDIR); \
    fi; \
    if [ -f $(TARGET_BUILD_PATH) ]; then \
	    rm $(TARGET_BUILD_PATH); \
    fi

install: $(INPUT_PIPE_PATH) $(OUTPUT_PIPE_PATH)
	@echo "Installing $(TARGET_APP):"
	$(V_AT)if [ ! -d $(INSTALL_DIR) ]; then \
		sudo mkdir $(INSTALL_DIR) ; \
	fi
	$(V_AT)sudo cp $(TARGET_BUILD_PATH) $(INSTALL_DIR)/$(TARGET_APP)
	$(V_AT)sudo chown :$(FB_GROUP) $(INSTALL_DIR)/$(TARGET_APP)
	$(V_AT)sudo chmod g+s $(INSTALL_DIR)/$(TARGET_APP)
	$(V_AT)sudo setcap -q cap_sys_ptrace+ep $(INSTALL_DIR)/$(TARGET_APP);


uninstall:
	@echo "Uninstalling $(TARGET_APP):"
	-$(V_AT)sudo rm $(INSTALL_DIR)/$(TARGET_APP)

$(PAINTERD_OBJECTS) :
	@echo "Compiling $(<F):"
	$(V_AT)mkdir -p $(OBJDIR)
	@if [ "$(VERBOSE)" == "1" ]; then \
        $(DAEMON_FRAMEWORK_DIR)/cleanPrint.sh '$(CXX) $(BUILD_FLAGS)'; \
        echo '    -o "$@" \'; \
        echo '    -c "$<"'; \
        echo ''; \
	fi
	@$(CXX) $(BUILD_FLAGS) -o "$@" -c "$<"

-include $(PAINTERD_OBJECTS:%.o=%.d)

$(OBJDIR)/Main.o: $(SOURCE_DIR)/Main.cpp
$(OBJDIR)/Cursor.o: $(CURSOR_CPP)
$(OBJDIR)/PainterLoop.o: $(SOURCE_DIR)/PainterLoop.cpp
