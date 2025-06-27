CC=gcc
CFLAGS=-Wall -Wextra -fanalyzer
LDFLAGS=-lrt -pthread
TARGET=program
SRC=main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run:
	./$(TARGET) test.txt

run-delete:
	./$(TARGET) test.txt -d 1

clean:
	rm -f $(TARGET) test.txt trace.log
