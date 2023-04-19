.PHONY: build clean rebuild test testout

build:
	./project_build.sh
clean:
	/bin/rm -r build/
rebuild: clean build
	echo "rebuilding..."

test:
	cd ./build && ctest -C Debug && cd ..
testout:
	cd ./build && ctest --output-on-failure -C Debug && cd ..
