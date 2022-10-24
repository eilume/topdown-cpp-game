.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	make

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=Release .. && \
	make

.PHONY: run
run:
	cd build && \
	./topdown-game

.PHONY: format
format:
	clang-format src/* -i

.PHONY: clean
clean:
	rm -r build