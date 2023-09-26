#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lg.c"

void modifyDNSRecord(char *input, int newValue) {
    // Tokenize string on ' 's
    char *first_word = strtok(input, " ");
    char *second_word = strtok(NULL, " ");
    char *third_word = strtok(NULL, " ");

    strtok(NULL, " ");
    char *fourth_word = "Securedomains.rpz";
    strtok(NULL, " ");
    char fifth_word[20]; // Assuming a maximum of 20 characters for the string
    // Using sprintf to convert int to string
    sprintf(fifth_word, "%d", newValue);
    strtok(NULL, " ");
    char *sixth_word = "3600";
    strtok(NULL, " ");
    char *seventh_word = "3600";

    char *eighth_word = strtok(NULL, " ");
    char *nineth_word = strtok(NULL, " ");

    if (nineth_word == NULL) {
      lg("ERROR: Cannot modify line: Words not expected");
      return;
    }

    // Create modified record
    char output[1024];

    strcpy(output, first_word);
    strcat(output, " ");
    strcat(output, second_word);
    strcat(output, " ");
    strcat(output, third_word);
    strcat(output, " ");
    strcat(output, fourth_word);
    strcat(output, " ");
    strcat(output, fifth_word);
    strcat(output, " ");
    strcat(output, sixth_word);
    strcat(output, " ");
    strcat(output, seventh_word);
    strcat(output, " ");
    strcat(output, eighth_word);
    strcat(output, " ");
    strcat(output, nineth_word);

    strcpy(input, output);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
      lg("ERROR: Incorrect number of arguments passed: %d", argc);
      return 1;
    }

    char *fname = argv[1];

    /* 
      Open file and do manipulations
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
        // TODO: look into perror
        perror("Error creating the temporary file");
        fclose(originalFile);
        return 1;
    }

    // Line number to be deleted
    int currentLine = 1;


    char buffer[1024]; // Adjust buffer size as needed

    // Add header information
    strcpy(buffer, "$TTL 300\n@ SOA localhost. Securedomains.rpz. 0 3600 3600 259200 300\n  NS  localhost.");

    fputs(buffer, tempFile);

    // Read and copy lines from the original file to the temporary file
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {
        
        // Delete all lines that start with #
        if (buffer[0] != '#') {
          // Add " CNAME ." to end of line
          buffer[strcspn(buffer, "\n")] = 0; // Remove newline
          strcat(buffer, " CNAME .\n");
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

    return 0;
}
