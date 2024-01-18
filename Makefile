export PROJECT_NAME=threading_library
# export CC=gcc
# export CXX=g++

.PHONY: build clean rebuild test testprint package run

build: package
	cd build && cmake ..  -DCMAKE_BUILD_TYPE=Debug  -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} --preset conan-debug
	cd build && cmake --build .
clean:
	/bin/rm -r build/
rebuild: clean build
	echo "clean + build successfull!"
test:
	cd ./build && ctest -C Debug && cd ..
testprint:
	cd ./build && ctest --rerun-failed --output-on-failure -C Debug && cd ..
package:
	mkdir -p build
	conan install . --output-folder=./build --build=missing --profile default

run: check_project_name check_project_exists
	./build/${PROJECT_NAME}

check_project_name:
	@if [ -z "${PROJECT_NAME}" ]; then \
		echo "Error: PROJECT_NAME is empty. Set a valid project name."; \
		exit 1; \
	fi
check_project_exists:
	@if [ ! -e ./build/${PROJECT_NAME} ]; then \
		echo "Error: ${PROJECT_NAME} executable not found. Run 'make build' first."; \
		exit 1; \
	fi
