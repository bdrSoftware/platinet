CC = g++
OPZ = -ggdb3 -Wall -Wextra -fPIC -std=c++11
INCLUDE_LIBPQ = -I/usr/include/postgresql

.PHONY : all-static all-dynamic clean test test-dynamic

all-static:
	make -C engine/ static
	make -C pqcc/ static
	make -C pgDatabase/ static

all-dynamic:
	make -C engine/ dynamic
	make -C pqcc/ dynamic
	make -C pgDatabase/ dynamic

test: all-static test/test_function.o test/test.cpp
	$(CC) $(OPZ) -c -o test/test.o test/test.cpp $(INCLUDE_LIBPQ)
	$(CC) -o test/test test/test.o test/test_function.o ./libpgdatabase.a ./libengine.a ./libpqcc.a -lpq -lpthread

test-dynamic: all-dynamic test/test_function.o test/test.cpp
	$(CC) $(OPZ) -c -o test/test.o test/test.cpp $(INCLUDE_LIBPQ)
	$(CC) -o test/test test/test.o test/test_function.o -Wl,-rpath,./ -L. -lengine -lpqcc -lpgdatabase -lpq -lpthread

test/test_function.o : test/test_function.hpp test/test_function.cpp
	$(CC) $(OPZ) -c -o test/test_function.o test/test_function.cpp

clean:
	make -C engine/ clean
	make -C pqcc/ clean
	make -C pgDatabase/ clean
	rm -f test/test.o test/test_function.o test/test

