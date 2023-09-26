# Makefile for compiling two different source files with custom options

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -fdiagnostics-color=always -g

# Source files and output executables
SOURCES =  script1/script.c script1/manipulate.c script2/script.c script2/manipulate.c
OUTPUTS = script1/script script1/manipulate script2/script script2/manipulate

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
