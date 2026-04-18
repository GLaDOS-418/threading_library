.PHONY: deps configure fix-header-guards build test examples clean

BUILD_DIR=_build/debug
HEADER_GUARD_FIXER=$(CURDIR)/BuildConfig/Tools/fix_header_guards.py

deps:
	conan install . --output-folder=$(BUILD_DIR) --build=missing \
		--profile:build=./conan.profile --profile:host=./conan.profile

configure: deps
	cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/$(BUILD_DIR)/conan_toolchain.cmake

fix-header-guards:
	python3 "$(HEADER_GUARD_FIXER)" .

build: configure fix-header-guards
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

examples: configure fix-header-guards
	cmake --build $(BUILD_DIR) --target examples

clean:
	rm -rf _build
