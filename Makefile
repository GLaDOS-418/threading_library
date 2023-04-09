.PHONY: build clean rebuild test

build:
	./setup.sh
clean:
	/bin/rm -r build/
rebuild: clean build
	echo "rebuilding..."

test:
	cd ./build && ctest --output-on-failure -C Debug && cd ..
