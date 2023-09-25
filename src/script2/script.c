#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

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

    printf("Copied: %s -> %s\n", srcPath, destPath);
}

int main(void)
{
  /* 
    Read in filenames and urls from urls.txt
  */

  char fnames[MAX_URLS][MAX_URL_LENGTH];
  char urls[MAX_URLS][MAX_URL_LENGTH];
  
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
        if (sscanf(line, "%s %s", fname, url) == 2) {
            strcpy(fnames[i], fname);
            strcpy(urls[i], url);
            i++;
        } else {
            printf("Invalid input format.\n");
            return 1;
        }
      }
  }
  
  // Close the file
  fclose(file);

  /*
    Download each file and rename
  */
  for (size_t i = 0; i < MAX_URLS; i++)
  {
    /* code */
    if (urls[i][0] != COMMENT_CHAR) {
      printf("%s\n", urls[i]);
      // Construct the wget command
      char command[MAX_LINE_LENGTH]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "wget -O ./downloads/%s %s", fnames[i], urls[i]);

      printf("%s\n\n", command);

      // Run the wget command
      int result = system(command);

      if (result == 0) {
          printf("Download successful.\n");
      } else {
          printf("Download failed.\n");
          // TODO: Ignore failed downloads for now
          //return 1;
      }
    }
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

      // Print the filename
      printf("Filename: %s\n", entry->d_name);

      // Run action on the file
      // Construct the command
      char command[MAX_LINE_LENGTH]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "./manipulate ./downloads/%s", entry->d_name);

      printf("%s\n\n", command);

      // Run the command
      int result = system(command);

      if (result == 0) {
          printf("Command successful.\n");
      } else {
          printf("Command failed.\n");
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

  return 0;
}