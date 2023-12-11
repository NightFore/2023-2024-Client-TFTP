// TP2_1_cmd_args.c

// ------------------- Header -------------------- //
// Libraries
#include <stdio.h>
#include <stdlib.h>

// Core Functions
void parseCmdArgs(int argc, char *argv[], char **host, char **file);



// -------------------- Operations -------------------- //
// Function to parse command line arguments
void parseCmdArgs(int argc, char *argv[], char **host, char **file) {
    // Check the number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Retrieve information from the command-line arguments
    *host = argv[1];
    *file = argv[2];
}



// -------------------- Main -------------------- //
int main(int argc, char *argv[]) {
    char *host;
    char *file;

    // Parse command line arguments
    parseCmdArgs(argc, argv, &host, &file);

    // Debug: Display information
    printf("Host: %s\n", host);
    printf("File: %s\n", file);

    return EXIT_SUCCESS;
}
