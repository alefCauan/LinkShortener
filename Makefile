CC = gcc
CFLAGS = -O2 -Wall -Wextra -pthread -Wno-sign-compare
LDFLAGS =
INCLUDES = -I.
SOURCES = main.c
TARGET = linkshortener

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
