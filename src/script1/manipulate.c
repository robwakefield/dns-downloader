#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
      printf("Incorrect number of arguments passed: %d\n", argc);
      return 1;
    }

    char *fname = argv[1];
    printf("Manipulating %s\n", fname);

    // Open file and do manipulations

    return 0;
}
