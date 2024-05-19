export PROJECT_NAME=project_name
export CC=clang
export CXX=clang++

# define docker names
IMAGE_NAME=cppdev
IMAGE_TAG=latest
CONTAINER_NAME=${IMAGE_NAME}_container

# Define the container home directory variable
CONTAINER_HOME_DIR=/home/${PROJECT_NAME}

.PHONY: build clean rebuild test testprint package run deepclean terminal image format

format:
	git ls-files -cmo --exclude-standard | grep -iE '\.(c|cc|cpp|cxx|txx|h|hpp|tpp)$$' | xargs clang-format -i
package:
	mkdir -p build
	conan install . \
		--output-folder=./build \
		--build=missing \
		--profile:build=./conan.profile \
		--profile:host=./conan.profile
build: format package
	mkdir -p logs
	@NOW=$$(date +"%Y%m%d_%H%M%S"); \
	LOG_FILE="buildlog_$${NOW}.txt"; \
	cd build && \
	cmake .. -DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} \
	--preset conan-debug 2>&1 | tee ../logs/$${LOG_FILE}; \
	cmake --build . 2>&1 | tee -a ../logs/$${LOG_FILE}
clean:
	/bin/rm -rf build/
deepclean: clean
	conan remove -c "*"
rebuild: clean build
	echo "clean + build successfull!"
test:
	cd ./build && ctest -C Debug && cd ..
testprint:
	cd ./build && \
		ctest --rerun-failed --output-on-failure -C Debug && cd ..
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
image:
	docker build -f ./.devcontainer/Dockerfile \
		--tag=${IMAGE_NAME}:${IMAGE_TAG} .
terminal:
	if [ $$(docker ps -aq -f name=${CONTAINER_NAME}) ]; then \
		docker start ${CONTAINER_NAME} || true; \
		docker exec -it ${CONTAINER_NAME} /bin/bash; \
	else \
		docker run --net=host -it \
			--name ${CONTAINER_NAME} \
			-v ${PWD}:${CONTAINER_HOME_DIR} \
 			-u $(id -u):$(id -g) \
			-w ${CONTAINER_HOME_DIR} \
			${IMAGE_NAME}:${IMAGE_TAG}; \
	fi
