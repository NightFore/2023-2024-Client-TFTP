#include <stdio.h>
#include <stdlib.h>

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