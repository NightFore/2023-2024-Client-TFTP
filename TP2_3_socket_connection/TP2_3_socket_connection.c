// TP2_3_socket_connection.c

// -------------------- Header -------------------- //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define constants for address family, socket type, protocol, and flags
#define DEFAULT_AI_FAMILY AF_INET    // Use IPv4 by default
#define DEFAULT_AI_SOCKTYPE 0        // Any socket type by default
#define DEFAULT_AI_PROTOCOL 0        // Any protocol by default
#define DEFAULT_AI_FLAGS 0           // No flags by default

// Function prototypes
void parseCmdArgs(int argc, char *argv[], char **host, char **file);
struct addrinfo* getAddressInfo(const char *host, const char *port);
void displayDebugHostFileInfo(const char *host, const char *file);
void displayDebugAddressInfo(const struct addrinfo *result);
int createSocketAndConnect(struct addrinfo *serverInfo);


// -------------------- Functions -------------------- //
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

// Function to get server address information using getaddrinfo
struct addrinfo* getAddressInfo(const char *host, const char *port) {
    struct addrinfo hints, *result;

    // Initialize hints to zero
    memset(&hints, 0, sizeof hints);

    // Set hints for IPv4, UDP, and default values for socktype, protocol, and flags
    hints.ai_family = DEFAULT_AI_FAMILY;
    hints.ai_socktype = DEFAULT_AI_SOCKTYPE;
    hints.ai_protocol = DEFAULT_AI_PROTOCOL;
    hints.ai_flags = DEFAULT_AI_FLAGS;

    // Get address information
    int status = getaddrinfo(host, port, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    return result;
}

// Function to reserve a connection socket to the server
int createSocketAndConnect(struct addrinfo *serverInfo) {
    int socket_fd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (socket_fd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}

// -------------------- Debug -------------------- //
// Function to display debug information about host and file
void displayDebugHostFileInfo(const char *host, const char *file) {
    printf("Host: %s\n", host);
    printf("File: %s\n", file);
}

// Function to display debug information about address details
void displayDebugAddressInfo(const struct addrinfo *result) {
    // Display address family
    printf("Address Family: %d\n", result->ai_family);

    // Display socket type
    printf("Socket Type: %d\n", result->ai_socktype);

    // Display protocol
    printf("Protocol: %d\n", result->ai_protocol);

    // Display flags
    printf("Flags: %d\n", result->ai_flags);

    // Display IP address as a string
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(result->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);
    printf("IP Address: %s\n", ipstr);
}



// -------------------- Main -------------------- //
int main(int argc, char *argv[]) {
    char *host;
    char *file;

    // Parse command line arguments
    parseCmdArgs(argc, argv, &host, &file);

    // Display information
    displayDebugHostFileInfo(host, file);

    // Get server address information using getaddrinfo
    struct addrinfo *result = getAddressInfo(host, "69");

    // Display address information
    displayDebugAddressInfo(result);

    // Create a socket and connect to the server
    int socket_fd = createSocketAndConnect(result);

    // Free the linked list of address info
    freeaddrinfo(result);

    // Close the socket
    close(socket_fd);

    return EXIT_SUCCESS;
}
