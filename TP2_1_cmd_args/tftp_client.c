#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Retrieve information from the command-line arguments
    char *host = argv[1];
    char *file = argv[2];

    // Display information
    printf("Host: %s\n", host);
    printf("File: %s\n", file);

    return EXIT_SUCCESS;
}
