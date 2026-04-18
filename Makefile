.PHONY: deps configure fix-header-guards build test examples iwyu-deps iwyu-configure iwyu iwyu-fix clean

BUILD_DIR=_build/debug
IWYU_BUILD_DIR=_build/iwyu
HEADER_GUARD_FIXER=$(CURDIR)/BuildConfig/Tools/fix_header_guards.py
IWYU_EXECUTABLE=$(shell bash -lc 'command -v include-what-you-use || command -v iwyu')
FIX_INCLUDES_EXECUTABLE=$(shell bash -lc 'command -v fix_includes.py || true')
IWYU_FIX_LOG=$(IWYU_BUILD_DIR)/iwyu.log
IWYU_FIX_ARGS ?= --nosafe_headers --ignore_re '/_build/'

# Install Conan dependencies for the default debug build tree.
deps:
	conan install . --output-folder=$(BUILD_DIR) --build=missing \
		--profile:build=./conan.profile --profile:host=./conan.profile

# Configure the default debug build tree with the Conan-generated toolchain.
configure: deps
	cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/$(BUILD_DIR)/conan_toolchain.cmake
	ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

# Normalize header guards before builds so include guards stay consistent.
fix-header-guards:
	python3 "$(HEADER_GUARD_FIXER)" .

# Build the default debug configuration for the library-owned targets.
build: configure fix-header-guards
	cmake --build $(BUILD_DIR)

# Build and run the GoogleTest suite in the default debug tree.
test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

# Build only the aggregated example applications from the default debug tree.
examples: configure fix-header-guards
	cmake --build $(BUILD_DIR) --target examples

# Install Conan dependencies for the dedicated IWYU analysis tree.
iwyu-deps:
	conan install . --output-folder=$(IWYU_BUILD_DIR) --build=missing \
		--profile:build=./conan.profile --profile:host=./conan.profile

# Configure the dedicated IWYU analysis tree and point CMake at the IWYU binary.
iwyu-configure: iwyu-deps
	cmake -S . -B $(IWYU_BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/$(IWYU_BUILD_DIR)/conan_toolchain.cmake \
		-DENABLE_IWYU=ON \
		-DPROJECT_IWYU_EXECUTABLE="$(IWYU_EXECUTABLE)"

# Run include-what-you-use diagnostics without modifying source files.
iwyu: iwyu-configure fix-header-guards
	cmake --build $(IWYU_BUILD_DIR) --clean-first

# Run IWYU, capture its diagnostics, and apply them with fix_includes.py.
# This stays separate from `make iwyu` because the fixer rewrites source files.
# Pass extra generic fixer behavior through `IWYU_FIX_ARGS`, for example:
# `make iwyu-fix IWYU_FIX_ARGS="--nosafe_headers --comments --update_comments"`
iwyu-fix: iwyu-configure fix-header-guards
	@if [ -z "$(FIX_INCLUDES_EXECUTABLE)" ]; then \
		echo "fix_includes.py was not found in PATH."; \
		echo "Install it or rerun with FIX_INCLUDES_EXECUTABLE=/absolute/path/to/fix_includes.py."; \
		exit 1; \
	fi
	@mkdir -p "$(IWYU_BUILD_DIR)"
	cmake --build $(IWYU_BUILD_DIR) --clean-first 2>&1 | tee "$(IWYU_FIX_LOG)"
	python3 "$(FIX_INCLUDES_EXECUTABLE)" $(IWYU_FIX_ARGS) < "$(IWYU_FIX_LOG)"

# Remove all generated build trees.
clean:
	rm -rf _build
