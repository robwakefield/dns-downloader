#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lg.c"


#define MAX_URLS 100
#define MAX_LINE_LENGTH 512
#define MAX_FNAME_LENGTH 256
#define MAX_URL_LENGTH 256

#define COMMENT_CHAR '#'


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
      lg("ERROR: Cannot modify line");
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
    int increment = 0;

    /* 
      Check in urls.txt for increment value
    */

    FILE *file;
    char line[MAX_URL_LENGTH];

    file = fopen("urls.txt", "r");

    if (file == NULL) {
      lg("Unable to open urls.txt");
      perror("Unable to open urls.txt");
      return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] != COMMENT_CHAR) {
          char dir_name[MAX_LINE_LENGTH];
          strcpy(dir_name, "./downloads/");
          char name[MAX_FNAME_LENGTH]; // Store the filename
          char url[MAX_URL_LENGTH]; // Store the URL
          int inc;

          // Use sscanf to split strings and get increment (if present)
          if (sscanf(line, "%s %s %d", name, url, &inc) == 3) {
            strcat(dir_name, name);
            if (!strcmp(fname, dir_name)) {
              increment = inc;
              break;
            }
          } else if (sscanf(line, "%s %s", name, url) == 2) {
            strcat(dir_name, name);
            if (!strcmp(fname, dir_name)) {
              increment = 0;
              break;
            }
          } else {
            continue;
          }
        }
    }
    
    // Close the file
    fclose(file);

    /* 
      Open file and do manipulations
    */

    // Open the original file for reading
    FILE *originalFile = fopen(fname, "r");
    if (originalFile == NULL) {
        lg("Error opening the original file");
        perror("Error opening the original file");
        return 1;
    }

    // Open a temporary file for writing
    FILE *tempFile = fopen("downloads/temp.txt", "w");
    if (tempFile == NULL) {
        lg("Error creating the temporary file");
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
              modifyDNSRecord(buffer, increment);
              //lg("Modified DNS Record: %s", buffer);
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
        lg("Error deleting the original file");
        perror("Error deleting the original file");
        return 1;
    }

    // Rename the temporary file to the original file
    if (rename("downloads/temp.txt", fname) != 0) {
        lg("Error renaming the temporary file");
        perror("Error renaming the temporary file");
        return 1;
    }

    lg("File manipulated successfully");

    return 0;
}
