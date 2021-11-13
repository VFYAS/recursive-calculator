C_MAIN_SOURCE=$(wildcard *.c)
LIBRARY=library.c variables.c
LIBRARY_OBJ=$(patsubst %.c, %.o, $(LIBRARY))
TESTS_MAIN=$(wildcard tests_main/tests/*)
TESTER_OBJ=tests_main/tester.o
TESTER_EX=$(patsubst %.o, %, $(TESTER_OBJ))
TEST_SCRIPT=test_lib.sh

CFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror \
       -Wformat-security -Wignored-qualifiers -Winit-self \
       -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith \
       -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes \
       -Wold-style-declaration -Wold-style-definition \
       -Wmissing-parameter-type -Wmissing-field-initializers \
       -Wnested-externs -Wno-pointer-sign -std=gnu11 -lm \
       -ggdb3 -Wno-unused-result -fsanitize=address -fsanitize=leak
CC=gcc
CVALFLAGS=-O2 -ftrapv -fsanitize=undefined -Wall -Werror \
	  -Wformat-security -Wignored-qualifiers -Winit-self \
	  -Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith \
	  -Wtype-limits -Wempty-body -Wlogical-op -Wstrict-prototypes \
	  -Wold-style-declaration -Wold-style-definition \
	  -Wmissing-parameter-type -Wmissing-field-initializers \
	  -Wnested-externs -Wno-pointer-sign -Wcast-qual -Wwrite-strings \
	  -std=gnu11 -lm
VALGR_FLAGS=--leak-check=full --show-leak-kinds=all \
	    --track-origins=yes --verbose
OBJS=$(patsubst %.c, %.o, $(C_MAIN_SOURCE))
PROGRAM=main

.PHONY: all clean run valcheck valcomp test_main_comp test_main build_library

all: valcomp

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) 

valcomp:
	$(CC) -o $(PROGRAM) $(C_MAIN_SOURCE) $(CVALFLAGS)

test_main_comp: $(TESTER_OBJ) $(PROGRAM)
	@$(CC) -o $(patsubst %.o, %, $<) $< $(CFLAGS)

test_main: test_main_comp
	@if [ ! -x $(TEST_SCRIPT) ]; then \
        chmod +x $(TEST_SCRIPT); \
    fi
	@./$(TEST_SCRIPT)

build_library: $(LIBRARY_OBJ)
	

build_library_val: $(LIBRARY)
	$(CC) -c -o $(LIBRARY_OBJ) $< $(CVALFLAGS)

run: $(PROGRAM)
	./$<

valcheck: valcomp 
	valgrind $(VALGR_FLAGS) ./$(PROGRAM)

clean:
	@rm -f *.o $(PROGRAM) tests_main/*.o $(TESTER_EX) tests_library/*.o tests_library/*.out
