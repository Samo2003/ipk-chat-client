CC = gcc
PYTHON = python3
CFLAGS = -std=c17 -Wextra -Wall -pedantic -g

TARGET = ipk-25-chat
LOGIN = xstefas00

TESTDIR = test
TEST = test.py

# Source files and header files
SRCS = $(wildcard src/*.c)
LIBS = $(wildcard lib/*.h)

# Object files
OBJS = $(SRCS:.c=.o)


all: $(TARGET)

# Build the target from object files
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Run tests
test: $(TARGET)
	$(PYTHON) $(TESTDIR)/$(TEST)

# Zip the project
zip: clean
	rm -rf $(LOGIN).zip
	zip -r $(LOGIN).zip ./ -x "*.git/*" "*.DS_Store"

# Remove object files and target
clean:
	rm -f $(TARGET)
	cd src && rm -f *.o

.PHONY: clean test zip