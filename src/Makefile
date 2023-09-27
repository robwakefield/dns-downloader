# Makefile for compiling two different source files with custom options

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -fdiagnostics-color=always -g

# Source files and output executables
SOURCES =  script1/script1.c script2/script2.c
OUTPUTS = script1/script1 script2/script2

# Default target
all: $(OUTPUTS)

# Compile each source file separately
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean generated files
clean:
	rm -f $(OUTPUTS)

# Phony targets
.PHONY: all clean
