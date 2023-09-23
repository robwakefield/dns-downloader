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

    /* 
      Open file and do manipulations
    */

    /* 
      DELETE higlighted lines
    */

    // Open the original file for reading
    FILE *originalFile = fopen(fname, "r");
    if (originalFile == NULL) {
        perror("Error opening the original file");
        return 1;
    }

    // Open a temporary file for writing
    FILE *tempFile = fopen("downloads/temp.txt", "w");
    if (tempFile == NULL) {
        perror("Error creating the temporary file");
        fclose(originalFile);
        return 1;
    }

    // Line number to be deleted
    int lineToDelete[3] = {5, 8, 9};
    int currentLine = 1;

    char buffer[1024]; // Adjust buffer size as needed

    // Read and copy lines from the original file to the temporary file
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {
        int delete = 0;
        for (int i = 0; i < 3; i++)
        {
          if (currentLine == lineToDelete[i]) {
            delete = 1;
          }
        }
        
        if (!delete) {
            fputs(buffer, tempFile);
        }
        currentLine++;
    }

    // Close both files
    fclose(originalFile);
    fclose(tempFile);

    // Delete the original file
    if (remove(fname) != 0) {
        perror("Error deleting the original file");
        return 1;
    }

    // Rename the temporary file to the original file
    if (rename("downloads/temp.txt", fname) != 0) {
        perror("Error renaming the temporary file");
        return 1;
    }

    printf("Lines deleted successfully.\n");

    /*
      CONTINUE with manipulations
    */

    return 0;
}
