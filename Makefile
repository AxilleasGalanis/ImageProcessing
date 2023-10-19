CC = g++
CFLAGS = -Wall -g -fsanitize=address
SRC = ImageProcessing.cpp
HEADER = ImageProcessing.hpp
EXECUTABLE = ImageProcessing

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC) $(HEADER)
	$(CC) $(CFLAGS) $(SRC) -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
