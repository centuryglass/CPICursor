# cursorKeyd
# Reads keyboard input to control a mouse cursor.

# The following variables should be passed in from the main project makefile:
#  - KD_TARGET_APP:  Daemon executable name
#  - KD_BUILD_DIR:   Daemon executable build directory
#  - KD_INSTALL_DIR: Daemon installation directory
#  - KD_PARENT_PATH: CPICursor installation path
#  - KD_PIPE_PATH:   Path to the pipe used to send key events to CPICursor
#  - KD_LOCK_PATH:   Path to the lock file used in controlling the pipe
#
# These variables are optional, but may be provided to override defaults:
#  - KD_CONFIG:
#  - KD_VERBOSE:

# Define relevant directories:
CURSOR_DAEMON_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR:=$(shell dirname $(realpath $(CURSOR_DAEMON_DIR)))
KEY_DAEMON_DIR:=$(PROJECT_DIR)/deps/KeyDaemon
DAEMON_FRAMEWORK_DIR:=$(PROJECT_DIR)/deps/DaemonFramework

# Define variables required by the main KeyDaemon makefile:
KD_CONFIG?=Debug
KD_VERBOSE?=1
KD_BUILD_DIR:=$(CURSOR_DAEMON_DIR)/build/$(KD_CONFIG)
KD_KEY_LIMIT=6

# Include main KeyDaemon makefile:
include $(KEY_DAEMON_DIR)/Makefile

.PHONY: default

default: $(KD_TARGET_PATH)
