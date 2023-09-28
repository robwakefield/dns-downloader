#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>

#define DEST_DIR "/etc/bind/master"

#define MAX_URLS 100
#define MAX_LINE_LENGTH 512
#define MAX_FNAME_LENGTH 256
#define MAX_URL_LENGTH 256

#define COMMENT_CHAR '#'

#define BUFFER_SIZE 1024

void lg(const char *format, ...) {
    // Get the current time and date
    time_t rawtime;
    struct tm *timeinfo;
    char timeStr[32]; // Adjust the size based on your formatting needs

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", timeinfo);

    // Print to the terminal
    printf("%s", timeStr);

    // Print to the log file
    FILE *logfile = fopen("../script1-logs.txt", "a");
    if (logfile == NULL) {
        perror("Failed to open log file");
        return;
    }
    fprintf(logfile, "%s", timeStr);

    // Print the formatted message to both the terminal and the log file
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args); // Print to terminal
    vfprintf(logfile, format, args); // Print to log file
    va_end(args);

    // Append a newline character to the log file
    fprintf(logfile, "\n");
    // Append a newline character to stdout
    fprintf(stdout, "\n");

    // Close the log file
    fclose(logfile);
}

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

int manipulate(char *fname) {

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

    int currentLine = 1;

    // Line to be edited
    int editLine = 2;

    char buffer[1024]; // Adjust buffer size as needed

    // Read and copy lines from the original file to the temporary file
    while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {

        // delete lines 4-9
        if ((currentLine < 4 || currentLine > 9)) {
            if (currentLine ==  14) {
              fputs("\n", tempFile);
            } else if (currentLine == editLine) {
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

void copyFile(const char *srcPath, const char *destPath) {
    int srcFile, destFile;
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    srcFile = open(srcPath, O_RDONLY);
    if (srcFile == -1) {
        lg("Failed to open source file");
        perror("Failed to open source file");
        return;
    }

    destFile = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destFile == -1) {
        lg("Failed to open destination file");
        perror("Failed to open destination file");
        close(srcFile);
        return;
    }

    while ((bytesRead = read(srcFile, buffer, BUFFER_SIZE)) > 0) {
        if (write(destFile, buffer, bytesRead) != bytesRead) {
            lg("Write error");
            perror("Write error");
            break;
        }
    }

    close(srcFile);
    close(destFile);

    if (bytesRead == -1) {
        lg("Read error");
        perror("Read error");
        return;
    }
}

int main(void)
{
  
  lg("Script 1 is starting");
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
    lg("Unable to open urls.txt");
    return 1;
  }

  int i = 0;
  while (fgets(line, sizeof(line), file) != NULL) {
      // Store each name and url in array
      if (line[0] != COMMENT_CHAR) {

        char fname[MAX_FNAME_LENGTH]; // Store the filename
        char url[MAX_URL_LENGTH]; // Store the URL

        // Use sscanf to split strings and set increment (if present)
        if (sscanf(line, "%s %s %d", fname, url, &increments[i]) == 3) {
          strcpy(fnames[i], fname);
          strcpy(urls[i], url);
          i++;
        } else if (sscanf(line, "%s %s", fname, url) == 2) {
          increments[i] = 0;
          strcpy(fnames[i], fname);
          strcpy(urls[i], url);
          i++;
        } else {
          lg("Invalid urls.txt format.");
          return 1;
        }
      }
  }
  
  // Close the file
  fclose(file);

  lg("Read from urls.txt");

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
      char command[1024]; // Adjust the buffer size as needed
      snprintf(command, sizeof(command), "wget -O ./downloads/%s %s &> /dev/null", fnames[i], urls[i]); // TODO: show errors if there are any

      lg("COMMAND: %s", command);

      // Run the wget command
      int result = system(command);

      if (result == 0) {
          lg("Download successful: %s", fnames[i]);
          increments[i]++;
      } else {
          lg("ERROR: Download failed: %s", urls[i]);
          // Ignore failed downloads
      }
    }
  }

  /*
    Update urls.txt with increments
  */

   // Open the original file for reading
    FILE *originalFile = fopen("urls.txt", "r");
    if (originalFile == NULL) {
        lg("Error opening the original file");
        perror("Error opening the original file");
        return 1;
    }

    // Open a temporary file for writing
    FILE *tempFile = fopen("urls.temp", "w");
    if (tempFile == NULL) {
        lg("Error creating the temporary file");
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
        lg("Error deleting the original file");
        perror("Error deleting the original file");
        return 1;
    }

    // Rename the temporary file to the original file
    if (rename("urls.temp", "urls.txt") != 0) {
        lg("Error renaming the temporary file");
        perror("Error renaming the temporary file");
        return 1;
    }

  // TODO: Perform actions on each file

  // Open the directory for reading
  DIR *dir = opendir("./downloads");

  if (dir == NULL) {
      lg("Unable to open directory");
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

      char long_name[MAX_LINE_LENGTH];
      strcpy(long_name, "./downloads/");
      strcat(long_name, entry->d_name);

      int result = manipulate(long_name);

      if (result == 0) {
          lg("%s manipulated successfully", entry->d_name);
      } else {
          lg("ERROR: manipulating %s", entry->d_name);
      }
  }

  // Close the directory
  closedir(dir);

  lg("All manipulations finished");

  /*
    Move files from ./downloads to user defined directory
  */
  
  const char *srcDir = "./downloads";
  const char *destDir = DEST_DIR;

  lg("Copying files to %s", destDir);

  // Open the source directory
  dir = opendir(srcDir);
  if (dir == NULL) {
      lg("Failed to open source directory");
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


  lg("Script 1 has finished");
  
  return 0;
}