#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_URLS 20
#define MAX_LINE_LENGTH 512
#define MAX_FNAME_LENGTH 256
#define MAX_URL_LENGTH 256

#define COMMENT_CHAR '#'

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
  
  return 0;
}