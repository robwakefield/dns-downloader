#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

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
