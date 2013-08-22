CC = g++
OPZ = -ggdb3 -Wall -Wextra -fPIC -std=c++11

.PHONY : all clean dynamic static test

dynamic: all
	g++ -shared -o ../libengine.so *.o

static: all
	ar rcs ../libengine.a *.o

all : time.o column.o tuple.o mStorage.o fStorage.o table.o schema.o database.o

clean :
	rm -f *.o ../libengine.*

column.o : column.cpp column.hpp
	$(CC) $(OPZ) -c -o column.o column.cpp

database.o : database.cpp database.hpp
	$(CC) $(OPZ) -c -o database.o database.cpp

fStorage.o: fStorage.cpp fStorage.hpp
	$(CC) $(OPZ) -c -o fStorage.o fStorage.cpp

mStorage.o: mStorage.cpp mStorage.hpp
	$(CC) $(OPZ) -c -o mStorage.o mStorage.cpp

schema.o : schema.cpp schema.hpp
	$(CC) $(OPZ) -c -o schema.o schema.cpp

table.o : table.cpp table.hpp
	$(CC) $(OPZ) -c -o table.o table.cpp

test: test.cpp static
	$(CC) $(OPZ) -c -o test.o test.cpp
	$(CC) -o test test.o ../libengine.a

time.o : time.cpp time.hpp
		$(CC) $(OPZ) -c -o time.o time.cpp

tuple.o: tuple.cpp tuple.hpp
	$(CC) $(OPZ) -c -o tuple.o tuple.cpp
