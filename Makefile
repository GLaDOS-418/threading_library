.PHONY: deps configure fix-header-guards build test examples iwyu-deps iwyu-configure iwyu iwyu-fix docs docs-serve format-cmake clean

BUILD_DIR=_build/debug
BUILD_GENERATORS_DIR=$(BUILD_DIR)/build/Debug/generators
IWYU_BUILD_DIR=_build/iwyu
IWYU_GENERATORS_DIR=$(IWYU_BUILD_DIR)/build/Debug/generators
DOCS_BUILD_DIR=_build/docs
DOCS_HTML_DIR=$(DOCS_BUILD_DIR)/html
DOCS_PORT ?= 8000
HEADER_GUARD_FIXER=$(CURDIR)/BuildConfig/Tools/fix_header_guards.py
IWYU_EXECUTABLE=$(shell bash -lc 'command -v include-what-you-use || command -v iwyu')
FIX_INCLUDES_EXECUTABLE=$(shell bash -lc 'command -v fix_includes.py || true')
CMAKE_FORMAT_EXECUTABLE=$(shell bash -lc 'command -v cmake-format || true')
DOXYGEN_EXECUTABLE=$(shell bash -lc 'command -v doxygen || true')
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
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/$(BUILD_GENERATORS_DIR)/conan_toolchain.cmake
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

# Generate the Doxygen HTML documentation into the dedicated docs build tree.
docs:
	@if [ -z "$(DOXYGEN_EXECUTABLE)" ]; then \
		echo "doxygen was not found in PATH."; \
		echo "Install it or rerun with DOXYGEN_EXECUTABLE=/absolute/path/to/doxygen."; \
		exit 1; \
	fi
	@mkdir -p "$(DOCS_BUILD_DIR)"
	"$(DOXYGEN_EXECUTABLE)" Docs/Doxyfile

# Serve the generated Doxygen HTML output with Python's built-in HTTP server.
docs-serve: docs
	python3 -m http.server "$(DOCS_PORT)" --directory "$(DOCS_HTML_DIR)"

# Format the repo's CMake files using the checked-in cmake-format policy.
format-cmake:
	@if [ -z "$(CMAKE_FORMAT_EXECUTABLE)" ]; then \
		echo "cmake-format was not found in PATH."; \
		echo "Install it or rerun with CMAKE_FORMAT_EXECUTABLE=/absolute/path/to/cmake-format."; \
		exit 1; \
	fi
	"$(CMAKE_FORMAT_EXECUTABLE)" -i \
		CMakeLists.txt \
		Library/CMakeLists.txt \
		Examples/CMakeLists.txt \
		Examples/SmokeApp/CMakeLists.txt \
		Tests/CMakeLists.txt \
		Tests/Src/CMakeLists.txt \
		BuildConfig/Cmake/*.cmake

# Install Conan dependencies for the dedicated IWYU analysis tree.
iwyu-deps:
	conan install . --output-folder=$(IWYU_BUILD_DIR) --build=missing \
		--profile:build=./conan.profile --profile:host=./conan.profile

# Configure the dedicated IWYU analysis tree and point CMake at the IWYU binary.
iwyu-configure: iwyu-deps
	cmake -S . -B $(IWYU_BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/$(IWYU_GENERATORS_DIR)/conan_toolchain.cmake \
		-DENABLE_IWYU=ON \
		-DPROJECT_IWYU_EXECUTABLE="$(IWYU_EXECUTABLE)"

# Run include-what-you-use diagnostics without modifying source files.
# NOTE: sometimes it might be required to clean first and then run this target.
iwyu: iwyu-configure fix-header-guards
	cmake --build $(IWYU_BUILD_DIR)

# Run IWYU, capture its diagnostics, and apply them with fix_includes.py.
# This stays separate from `make iwyu` because the fixer rewrites source files.
# Pass extra generic fixer behavior through `IWYU_FIX_ARGS`, for example:
# `make iwyu-fix IWYU_FIX_ARGS="--nosafe_headers --comments --update_comments"`
# NOTE: sometimes it might be required to clean first and then run this target.
iwyu-fix: iwyu-configure fix-header-guards
	@if [ -z "$(FIX_INCLUDES_EXECUTABLE)" ]; then \
		echo "fix_includes.py was not found in PATH."; \
		echo "Install it or rerun with FIX_INCLUDES_EXECUTABLE=/absolute/path/to/fix_includes.py."; \
		exit 1; \
	fi
	@mkdir -p "$(IWYU_BUILD_DIR)"
	cmake --build $(IWYU_BUILD_DIR) 2>&1 | tee "$(IWYU_FIX_LOG)"
	python3 "$(FIX_INCLUDES_EXECUTABLE)" $(IWYU_FIX_ARGS) < "$(IWYU_FIX_LOG)"

# Remove all generated build trees.
clean:
	rm -rf _build
