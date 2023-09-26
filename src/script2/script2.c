#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include "lg.c"

#define MAX_URLS 100
#define MAX_LINE_LENGTH 512
#define MAX_FNAME_LENGTH 256
#define MAX_URL_LENGTH 256

#define COMMENT_CHAR '#'

#define BUFFER_SIZE 1024

void copyFile(const char *srcPath, const char *destPath) {
    int srcFile, destFile;
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    srcFile = open(srcPath, O_RDONLY);
    if (srcFile == -1) {
        perror("Failed to open source file");
        return;
    }

    destFile = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destFile == -1) {
        perror("Failed to open destination file");
        close(srcFile);
        return;
    }

    while ((bytesRead = read(srcFile, buffer, BUFFER_SIZE)) > 0) {
        if (write(destFile, buffer, bytesRead) != bytesRead) {
            perror("Write error");
            break;
        }
    }

    close(srcFile);
    close(destFile);

    if (bytesRead == -1) {
        perror("Read error");
        return;
    }

}

int main(void)
{
  
  lg("Script 2 is starting");
  /* 
    Read in filenames and urls from urls.txt
  */

  char fnames[MAX_URLS][MAX_URL_LENGTH];
  char urls[MAX_URLS][MAX_URL_LENGTH];
  int increments[MAX_URLS];
  
  for (size_t i = 0; i < MAX_URLS; i++)
  {
    fnames[i][0] = COMMENT_CHAR;
    urls[i][0] = COMMENT_CHAR;
  }

  FILE *file;
  char line[MAX_URL_LENGTH];

  file = fopen("urls.txt", "r");

  if (file == NULL) {
    perror("Unable to open urls.txt");
    return 1;
  }

  //Read and print all lines
  int i = 0;
  while (fgets(line, sizeof(line), file) != NULL) {
      // Store each name and url in array
      if (line[0] != COMMENT_CHAR) {

        char fname[MAX_FNAME_LENGTH]; // Store the filename
        char url[MAX_URL_LENGTH]; // Store the URL

        // Use sscanf to split the input into two strings
        if (sscanf(line, "%s %s %d", fname, url, &increments[i]) == 3) {
          strcpy(fnames[i], fname);
          strcpy(urls[i], url);
          i++;
        } else if (sscanf(line, "%s %s", fname, url) == 2) {
          increments[i] = -1;
          strcpy(fnames[i], fname);
          strcpy(urls[i], url);
          i++;
        } else {
            lg("ERROR: Invalid input format.");
            return 1;
        }
      }
  }
  
  // Close the file
  fclose(file);

  /*
    Download each file and rename
  */
  
  // Create the destination directory if it doesn't exist
  mkdir("./downloads", 0755); 
    
  for (size_t i = 0; i < MAX_URLS; i++)
  {
    /* code */
    if (urls[i][0] != COMMENT_CHAR) {
      lg("Downloading %s", urls[i]);
      // Construct the wget command
      char command[MAX_LINE_LENGTH]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "wget -O ./downloads/%s %s &> /dev/null", fnames[i], urls[i]);

      lg("COMMAND: %s", command);

      // Run the wget command
      int result = system(command);

      if (result == 0) {
          lg("Download successful: %s", fnames[i]);
          increments[i]++;
      } else {
          lg("Download failed: %s", urls[i]);
          // TODO: Ignore failed downloads for now
          //return 1;
      }
    }
  }

  /*
    Update urls.txt with increments
  */

    // Open the original file for reading
    FILE *originalFile = fopen("urls.txt", "r");
    if (originalFile == NULL) {
        perror("Error opening the original file");
        return 1;
    }

    // Open a temporary file for writing
    FILE *tempFile = fopen("urls.temp", "w");
    if (tempFile == NULL) {
        perror("Error creating the temporary file");
        fclose(originalFile);
        return 1;
    }

    int currentLine = 1;
    char buffer[1024]; // Adjust buffer size as needed

    // Read and copy lines from the original file to the temporary file
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {
        if (buffer[0] != '#') {
            // Remove previous incremen
            char *firstSpace = strchr(buffer, ' '); // Find the first space in the string
            char *lastSpace = strrchr(buffer, ' '); // Find the last space in the string
            // If first and last space are the same (the only space) we don't want to remove
            if (firstSpace != lastSpace && lastSpace != NULL) {
                // Null-terminate the string at the last space to remove the last word
                *lastSpace = '\0';
            }

            char inc_str[20]; // Assuming a maximum of 20 characters for the string
            // Using sprintf to convert int to string
            sprintf(inc_str, " %d\n", increments[currentLine - 1]);
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline
            strcat(buffer, inc_str);
            currentLine++;
        }
        fputs(buffer, tempFile);
    }

    // Close both files
    fclose(originalFile);
    fclose(tempFile);

    // Delete the original file
    if (remove("urls.txt") != 0) {
        perror("Error deleting the original file");
        return 1;
    }

    // Rename the temporary file to the original file
    if (rename("urls.temp", "urls.txt") != 0) {
        perror("Error renaming the temporary file");
        return 1;
    }

  // TODO: Perform actions on each file

  // Open the directory for reading
  DIR *dir = opendir("./downloads");

  if (dir == NULL) {
      perror("Unable to open directory");
      return 1;
  }

  struct dirent *entry;

  // Read each entry in the directory
  while ((entry = readdir(dir)) != NULL) {
      // Skip "." and ".." entries
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
          continue;
      }

      // Run action on the file
      lg("Manipulating %s", entry->d_name);
      // Construct the command
      char command[MAX_LINE_LENGTH]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "./manipulate2 ./downloads/%s", entry->d_name);

      // Run the command
      int result = system(command);

      if (result == 0) {
          lg("%s manipulated successfully", entry->d_name);
      } else {
          lg("ERROR: manipulating %s", entry->d_name);
          // TODO: Ignore failed commands for now
          //return 1;
      }
  }

  // Close the directory
  closedir(dir);

  /*
    Move files from ./downloads to user defined directory
  */
  
  const char *srcDir = "./downloads";
  const char *destDir = "/etc/bind/master";

  lg("Copying files to %s", destDir);

  // Open the source directory
  dir = opendir(srcDir);
  if (dir == NULL) {
      perror("Failed to open source directory");
      return 1;
  }

  // Create the destination directory if it doesn't exist
  mkdir(destDir, 0755);

  // Iterate over files in the source directory
  while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG) { // Regular file
          char srcPath[PATH_MAX];
          char destPath[PATH_MAX];

          snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, entry->d_name);
          snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);

          copyFile(srcPath, destPath);
      }
  }

  closedir(dir);

  /*
    Run 'sudo rndc reload'
  */

      // Construct the command
      char command[MAX_LINE_LENGTH]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "sudo rndc reload");

      lg("COMMAND: %s", command);

      // Run the command
      int result = system(command);

      if (result == 0) {
          lg("Command ran successfully");
      } else {
          lg("ERROR: running command");
      }

  lg("Script 2 has finished");

  return 0;
}