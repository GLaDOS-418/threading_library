build:
	./setup.sh
clean:
	/bin/rm -r build/
rebuild: clean build
	echo "rebuilding..."
