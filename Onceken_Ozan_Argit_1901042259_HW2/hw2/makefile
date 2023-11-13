CC = g++
CFLAGS = -std=c++11 -Wall

TARGET = program
SOURCES = main.cpp PageEntity.cpp VirtualAddressEntity.cpp VirtualMemoryManagement.cpp
HEADERS = PageEntity.h VirtualAddressEntity.h VirtualMemoryManagement.h VirtualMemoryManagement.h
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

clean-all:
	rm -f $(OBJECTS) $(TARGET)
