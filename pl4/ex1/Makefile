# Makefile for Shared Memory Writer/Reader Example

# Compiler
CC = gcc

# Compiler flags:
# -Wall : Enable all standard warnings
# -Wextra : Enable extra warnings beyond -Wall
# -g : Generate debugging information (useful for gdb)
# -std=c11 : Use the C11 standard (adjust if needed, e.g., c99)
CFLAGS = -Wall -Wextra -g -std=c11

# Linker flags (not strictly necessary here, but good practice)
LDFLAGS =

# Libraries needed:
# -lrt : POSIX Real-time extensions library (required for shm_open, shm_unlink)
LDLIBS = -lrt

# Source files
WRITER_SRC = writer.c
READER_SRC = reader.c

# Object files (derived from source files)
WRITER_OBJ = $(WRITER_SRC:.c=.o)
READER_OBJ = $(READER_SRC:.c=.o)

# Target executables
WRITER_TARGET = writer
READER_TARGET = reader
TARGETS = $(WRITER_TARGET) $(READER_TARGET)

# Header file dependency
HEADERS = defs.h

# Default target: build all executables
# This is the rule executed when you just type 'make'
.PHONY: all
all: $(TARGETS)

# Rule to link the writer executable
$(WRITER_TARGET): $(WRITER_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
# $^ means "all prerequisites" (writer.o)
# $@ means "the target file" (writer)

# Rule to link the reader executable
$(READER_TARGET): $(READER_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
# $^ means "all prerequisites" (reader.o)
# $@ means "the target file" (reader)

# Rule to compile writer object file
# It depends on the source file and the header file
$(WRITER_OBJ): $(WRITER_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
# $< means "the first prerequisite" (writer.c)
# $@ means "the target file" (writer.o)
# -c means "compile only, do not link"

# Rule to compile reader object file
# It depends on the source file and the header file
$(READER_OBJ): $(READER_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
# $< means "the first prerequisite" (reader.c)
# $@ means "the target file" (reader.o)

# Clean target: remove generated files
# .PHONY tells make that 'clean' is not a file to be built
.PHONY: clean
clean:
	# rm -f: remove forcefully (ignore errors if files don't exist)
	rm -f $(TARGETS) $(WRITER_OBJ) $(READER_OBJ) *.o *~ core core.*