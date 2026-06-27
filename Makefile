BUILD_DIR := build
BIN := $(BUILD_DIR)/main

.PHONY: all build run clean configure

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR) -j

run: build
	cd $(BUILD_DIR) && ./main

clean:
	rm -rf $(BUILD_DIR)
