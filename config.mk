# Build configuration
KERNEL_VERSION := $(shell uname -r)
LINUX_BUILD_DIR := /lib/modules/$(KERNEL_VERSION)/build

# Compiler settings
CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2

# Installation paths
PREFIX := /usr/local
INSTALL_BIN := $(PREFIX)/bin
INSTALL_MOD := /lib/modules/$(KERNEL_VERSION)/extra
