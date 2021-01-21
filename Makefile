CC=g++-10

FLAGS=-std=c++2a -ggdb -Wall -Wunused-function -Wextra -Werror

INCLUDE_DIRS = src ./boost_1_74_0
INCLUDE= $(foreach p, $(INCLUDE_DIRS), -I$p)
LIBS =

BUILD = build

.PHONY: main clean tests format interleave
all: main
tests: compile_and_run_tests

folders:
	mkdir -p build

all: format interleave

format: folders
	$(CC) format.cpp $(LIBS) $(FLAGS) $(INCLUDE) -o build/format

interleave: folders
	$(CC) interleave.cpp $(LIBS) $(FLAGS) $(INCLUDE) -o build/interleave

compile_and_run_tests:
	$(CC) tests/tests.cpp -lcmocka $(INCLUDE) -ggdb -o build/tests
	./build/tests

clean:
	rm -rf build

vpath %.cpp src

