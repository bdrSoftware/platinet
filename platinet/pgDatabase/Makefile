CC = g++
OPZ = -ggdb3 -Wall -Wextra -fPIC -std=c++11
INCLUDE_LIBPQ = -I/usr/include/postgresql

.PHONY : all clean dynamic static test

dynamic: all
	g++ -shared -o ../libpgdatabase.so *.o

static: all
	ar rcs ../libpgdatabase.a *.o

all : pgDatabase.o

clean :
	rm -f *.o ../libpgdatabase.*

pgDatabase.o : pgDatabase.cpp pgDatabase.hpp
	$(CC) $(OPZ) -c -o pgDatabase.o pgDatabase.cpp	$(INCLUDE_LIBPQ)

