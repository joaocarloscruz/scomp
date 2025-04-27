CC = gcc
CFLAGS = -Wall -Wextra -g -MMD -MP
LDFLAGS = -lrt

WRITER_EXEC = writer
READER_EXEC = reader

WRITER_OBJS = writer.o
READER_OBJS = reader.o

DEPS = $(WRITER_OBJS:.o=.d) $(READER_OBJS:.o=.d)

all: $(WRITER_EXEC) $(READER_EXEC)

$(WRITER_EXEC): $(WRITER_OBJS)
	$(CC) $(CFLAGS) $(WRITER_OBJS) -o $@ $(LDFLAGS)

$(READER_EXEC): $(READER_OBJS)
	$(CC) $(CFLAGS) $(READER_OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: all clean

clean:
	rm -f $(WRITER_OBJS) $(READER_OBJS) $(WRITER_EXEC) $(READER_EXEC) $(DEPS)