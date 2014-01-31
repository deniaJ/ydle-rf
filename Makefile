CC=g++
CFLAGS=-c -Wall -D RASPBERRY -std=c++0x -fpermissive

LDFLAGS=  -lwiringPi  -lpthread -llua5.1

SOURCES=src/main.cpp src/logging.cpp src/protocolRF.cpp src/LuaStack.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ydle-rf

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

src/main.o: src/main.cpp src/protocolRF.h 
src/logging.o: src/logging.h 
src/protocolRF.o:  src/protocolRF.h
src/LuaStack.o: src/LuaStack.h

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*.o
	rm -rf $(EXECUTABLE)
	
