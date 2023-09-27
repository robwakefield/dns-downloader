#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lg.c"

#define MAX_URLS 100
#define MAX_LINE_LENGTH 512
#define MAX_FNAME_LENGTH 256
#define MAX_URL_LENGTH 256

#define COMMENT_CHAR '#'

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
    int currentLine = 1;


    char buffer[1024]; // Adjust buffer size as needed

    // Add header information
    strcpy(buffer, "$TTL 300\n@ SOA localhost. Securedomains.rpz. ");
    char inc_char[20]; // Assuming a maximum of 20 characters for the string
    // Using sprintf to convert int to string
    sprintf(inc_char, "%d", increment);
    strcat(buffer, inc_char);
    strcat(buffer, " 3600 3600 259200 300\n  NS  localhost.\n");

    fputs(buffer, tempFile);

    // Read and copy lines from the original file to the temporary file
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {
        
        // Delete all lines that start with #
        if (buffer[0] != '#' && buffer[0] != '\n') {
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
