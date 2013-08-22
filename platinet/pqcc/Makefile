CC = g++
OPZ = -ggdb3 -Wall -Wextra -fPIC -std=c++11
INCLUDE_LIBPQ = -I/usr/include/postgresql

.PHONY : all clean dynamic static test

dynamic: all
	g++ -shared -o ../libpqcc.so *.o

static: all
	ar rcs ../libpqcc.a *.o

all : pgConnection.o pgQuery.o pgDriver.o

clean :
	rm -f *.o ../libpqcc.*

pgConnection.o : pgConnection.cpp pgConnection.hpp
	$(CC) $(OPZ) -c -o pgConnection.o pgConnection.cpp $(INCLUDE_LIBPQ)

pgDriver.o : pgDriver.cpp pgDriver.hpp
	$(CC) $(OPZ) -c -o pgDriver.o pgDriver.cpp $(INCLUDE_LIBPQ)

pgQuery.o : pgQuery.cpp pgQuery.hpp
	$(CC) $(OPZ) -c -o pgQuery.o pgQuery.cpp $(INCLUDE_LIBPQ)
