// TP2_2_getaddrinfo.c

/*
    Changes from the previous code:

    - Added header files (arpa/inet.h, netdb.h, string.h, sys/socket.h, sys/types.h).
    - Added new constants (TFTP_SERVER_PORT, AI_FAMILY, AI_SOCKTYPE, AI_PROTOCOL, AI_FLAGS).
    - Added new functions (cleanup, getAddressInfo, displayDebugAddressInfo).
*/

// -------------------- Header -------------------- //
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

// Constants
#define TFTP_SERVER_PORT "69"       // Default port number for TFTP server
#define AI_FAMILY AF_INET           // Use IPv4 address family by default
#define AI_SOCKTYPE SOCK_DGRAM      // Datagram socket type for UDP
#define AI_PROTOCOL IPPROTO_UDP     // UDP protocol for socket
#define AI_FLAGS 0                  // No special flags for getaddrinfo function

// Helper Functions
void handle_error(const char *location, const char *message, const char *perror_message);
void cleanup(struct addrinfo *serverAddr);

// Core Functions
void parseCmdArgs(int argc, char *argv[], char **host, char **file);
struct addrinfo* getAddressInfo(const char *host, const char *port);

// Debug Functions
void displayDebugHostFileInfo(const char *host, const char *file);
void displayDebugAddressInfo(const struct addrinfo *serverAddr);



// -------------------- Helper Functions -------------------- //
// Function to handle errors
void handle_error(const char *location, const char *message, const char *perror_message) {
    // Print an error message with location and custom message
    fprintf(stderr, "Error at %s: %s\n", location, message);

    // Check if a custom perror message is provided
    if (perror_message != NULL) {
        // Print the perror message for the specific error code
        perror(perror_message);
    }

    // Exit the program with a failure status
    exit(EXIT_FAILURE);
}

// Function to perform cleanup before exiting the program
void cleanup(struct addrinfo *serverAddr) {
    // Free the linked list of address info
    if (serverAddr != NULL) {
        freeaddrinfo(serverAddr);
    }
}



// -------------------- Core Functions -------------------- //
// Function to parse command line arguments
void parseCmdArgs(int argc, char *argv[], char **host, char **file) {
    // Check the number of arguments
    if (argc != 3) {
        handle_error("parseCmdArgs", "Usage: <host> <file>", "argc");
    }

    // Retrieve information from the command-line arguments
    *host = argv[1];
    *file = argv[2];

    // Display host information
    displayDebugHostFileInfo(*host, *file);
}

// Function to get server address information using getaddrinfo
struct addrinfo* getAddressInfo(const char *host, const char *port) {
    struct addrinfo hints, *serverAddr;

    // Initialize hints to zero
    memset(&hints, 0, sizeof hints);

    // Set hints for address family , socket type, protocol, and no special flags
    hints.ai_family = AI_FAMILY;
    hints.ai_socktype = AI_SOCKTYPE;
    hints.ai_protocol = AI_PROTOCOL;
    hints.ai_flags = AI_FLAGS;

    // Get address information
    int status = getaddrinfo(host, port, &hints, &serverAddr);
    if (status != 0) {
        handle_error("getaddrinfo", "Failed to retrieve address information", gai_strerror(status));
    }

    // Display address information
    displayDebugAddressInfo(serverAddr);

    return serverAddr;
}


// -------------------- Debug -------------------- //
// Function to display debug information about host and file
void displayDebugHostFileInfo(const char *host, const char *file) {
    printf("----- parseCmdArgs -----\n");
    printf("Host: %s\n", host);
    printf("File: %s\n", file);
    printf("\n");
}

// Function to display debug information about address details
void displayDebugAddressInfo(const struct addrinfo *serverAddr) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)serverAddr->ai_addr;
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(serverAddr->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);

    printf("----- getAddressInfo -----\n");
    printf("Address Family: %d\n", serverAddr->ai_family);
    printf("Socket Type: %d\n", serverAddr->ai_socktype);
    printf("Protocol: %d\n", serverAddr->ai_protocol);
    printf("Flags: %d\n", serverAddr->ai_flags);
    printf("IP Address: %s\n", ipstr);
    printf("\n");
}



// -------------------- Main -------------------- //
int main(int argc, char *argv[]) {
    char *host;
    char *file;

    // Parse command line arguments
    parseCmdArgs(argc, argv, &host, &file);

    // Get server address information using getaddrinfo
    struct addrinfo *serverAddr = getAddressInfo(host, TFTP_SERVER_PORT);

    // Cleanup before exiting the program
    cleanup(serverAddr);

    // Exit the program successfully
    return EXIT_SUCCESS;
}
