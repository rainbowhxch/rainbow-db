CPP_SOURCES = $(wildcard src/*.cpp)
CPP_HEADERS = $(wildcard inc/*.h)
OBJ = ${CPP_SOURCES:.cpp=.o}

CC = g++
GDB = gdb
# -g: Use debugging symbols in g++
CPP_FLAGS = -g

rainbow-db.exe: $(OBJ)
	$(CC) $^ -o $@

%.o: %.c $(CPP_HEADERS)
	$(CC) -c $< -o $@ $(CPP_FLAGS)

clean:
	rm -rf src/*.o
	rm -rf *.exe

clean-db:
	rm -rf *.db
