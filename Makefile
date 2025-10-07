CC = gcc
CFLAGS = -O2 -Wall -Wextra -pthread -Wno-sign-compare
LDFLAGS =
# Put the path to your CWebStudio and DoTheWorld files in INCLUDES or copy them into the project dir.
INCLUDES = -I.
SOURCES = main.c
TARGET = linkshortener

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
