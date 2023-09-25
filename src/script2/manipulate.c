#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
      printf("Cannot modify line: Words not expected\n");
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
  // TODO: return nothing for testing
  return 0;

    if (argc != 2) {
      printf("Incorrect number of arguments passed: %d\n", argc);
      return 1;
    }

    char *fname = argv[1];
    printf("Manipulating %s\n", fname);

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
        perror("Error creating the temporary file");
        fclose(originalFile);
        return 1;
    }

    // Line number to be deleted
    int lineToDelete[3] = {5, 8, 9};
    int currentLine = 1;

    // Line to be edited
    int editLine = 2;

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
            if (currentLine == editLine) {
              // Edit the 2nd line and write it to file
              //printf("Modifying DNS Record: %s", buffer);
              modifyDNSRecord(buffer, 3);
              printf("Modified DNS Record: %s", buffer);
              fputs(buffer, tempFile);
            } else {
              fputs(buffer, tempFile);
            }
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

    return 0;
}