// TP2_5_puttftp.c

// -------------------- Header -------------------- //
// Libraries
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Constants
#define TFTP_SERVER_PORT "69"       // Default TFTP server port
#define DEFAULT_AI_FAMILY AF_INET   // Use IPv4 by default
#define DEFAULT_AI_SOCKTYPE 0       // Any socket type by default
#define DEFAULT_AI_PROTOCOL 0       // Any protocol by default
#define DEFAULT_AI_FLAGS 0          // No flags by default
#define RRQ_OPCODE_READ 1           // Read Request opcode
#define MODE_STRING "octet"         // Default Mode for file transfer
#define SENDTO_FLAGS 0              // No special flags for the sendto function
#define RECV_FLAGS 0                // No special flags for the recv function

// Structure definitions
struct ACKPacket {
    uint16_t opcode;                // Operation code
    uint16_t blockNumber;           // Block number
};

typedef struct {
    short opcode;                       // Operational code
    char *data;
}TFTP_Packet;

// Helper Functions
void handle_error(const char *location, const char *message, const char *perror_message);
void cleanup(int sockfd, struct addrinfo *serverAddr, char *rrqPacket, char *wrqPacket);

// Core Functions
void parseCmdArgs(int argc, char *argv[], char **host, char **file);
struct addrinfo* getAddressInfo(const char *host, const char *port);
int createSocket(const struct addrinfo *serverAddr);
char* sendRRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename);
char* sendWRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename);

// Debug Functions
void displayDebugHostFileInfo(const char *host, const char *file);
void displayDebugAddressInfo(const struct addrinfo *serverAddr);
void displayDebugSocketCreation(int sockfd);
void displayDebugRRQSuccess();
void displayDebugWRQSuccess();



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
void cleanup(int sockfd, struct addrinfo *serverAddr, char *rrqPacket, char *wrqPacket) {
    // Close the socket
    if (sockfd != -1) {
        close(sockfd);
    }

    // Free the linked list of address info
    if (serverAddr != NULL) {
        freeaddrinfo(serverAddr);
    }

    // Free the allocated memory for the RRQ packet
    if (rrqPacket != NULL) {
        free(rrqPacket);
    }

    // Free the allocated memory for the WRQ packet
    if (wrqPacket != NULL) {
        free(wrqPacket);
    }
}



// -------------------- Operations -------------------- //
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

    // Set hints for IPv4, UDP, and default values for socktype, protocol, and flags
    hints.ai_family = DEFAULT_AI_FAMILY;
    hints.ai_socktype = DEFAULT_AI_SOCKTYPE;
    hints.ai_protocol = DEFAULT_AI_PROTOCOL;
    hints.ai_flags = DEFAULT_AI_FLAGS;

    // Get address information
    int status = getaddrinfo(host, port, &hints, &serverAddr);
    if (status != 0) {
        handle_error("getaddrinfo", "Failed to retrieve address information", gai_strerror(status));
    }

    // Display address information
    displayDebugAddressInfo(serverAddr);

    return serverAddr;
}

// Function to create and reserve a socket for connection to the server
int createSocket(const struct addrinfo *serverAddr) {
    // Create a socket
    int sockfd = socket(serverAddr->ai_family, serverAddr->ai_socktype, serverAddr->ai_protocol);
    if (sockfd == -1) {
        handle_error("createSocket", "Failed to create socket", "socket");
    }

    // Display socket information
    displayDebugSocketCreation(sockfd);

    return sockfd;
}

// Function to send a RRQ (Read Request) to the server
char* sendRRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename) {
    // Format of a RRQ packet: opcode (2 bytes) + filename (variable) + \0 (1 byte) + "octet" (5 bytes) + 0 (1 byte)
    // Total minimum size: 9 bytes

    // Calculate the size of the RRQ packet
    size_t packetSize = sizeof(uint16_t) + strlen(filename) + 1 + strlen("octet") + 1;

    // Allocate memory for the RRQ packet
    char *rrqPacket = (char *) malloc(packetSize);
    if (rrqPacket == NULL) {
        handle_error("sendRRQ", "Failed to allocate memory for RRQ packet", "malloc");
    }

    // 1. Set the opcode for Read Request (RRQ)
    rrqPacket[0] = 0;
    rrqPacket[1] = 1;

    // 2. Copy the filename to the packet
    strcpy(rrqPacket + sizeof(uint16_t), filename);

    // 3. Add a null byte after the filename
    rrqPacket[sizeof(uint16_t) + strlen(filename)] = '\0';

    // 4. Copy the mode ("octet") to the packet
    strcpy(rrqPacket + sizeof(uint16_t) + strlen(filename) + 1, "octet");

    // 5. Add a null byte after the mode
    rrqPacket[sizeof(uint16_t) + strlen(filename) + 1 + strlen("octet")] = '\0';

    // Send the RRQ packet to the server
    ssize_t bytesSent = sendto(sockfd, rrqPacket, packetSize, 0, serverAddr, sizeof(struct sockaddr));
    if (bytesSent == -1) {
        free(rrqPacket);
        handle_error("sendRRQ", "Failed to send RRQ packet to the server", "sendto");
    }

    // Display a success message for the RRQ packet transmission
    displayDebugRRQSuccess();

    return rrqPacket;
}

// Function to send a WRQ (Write Request) to the server
char* sendWRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename){
    TFTP_Packet *packet;

    // Calculate the size of the WRQ packet
    size_t data_size = strlen(filename) + 1 + strlen("octet") + 1;

    // Allocate memory for the data field
    packet->data = (char *) malloc(data_size);
    if (packet->data == NULL) {
        handle_error("sendWRQ", "Failed to allocate memory for WRQ packet", "malloc");
    }

    // Create the WRQ

    // 1. Set 2 as opcode for WRQ
    packet->opcode = htons(2);

    // 2. Copy the filename to the packet
    strcpy(packet->data, filename);

    // 3. Add a null byte after the filename
    strcat(packet->data, "\0");

    // 4. Copy the mode ("octet") to the packet
    strcat(packet->data, "octet");

    // 5. Add a null byte after the filename
    strcat(packet->data, "\0");

    // Send WRQ request to server
    ssize_t bytesSent = sendto(sockfd, packet->data, data_size, DEFAULT_AI_FLAGS, serverAddr, sizeof(struct sockaddr));
    if (bytesSent == -1) {
        free(packet->data);
        handle_error("sendWRQ", "Failed to send WRQ packet to the server", "sendto");
    }

    // Display a success message for the RRQ packet transmission
    displayDebugWRQSuccess();

    return packet->data;
}



// -------------------- Debug -------------------- //
// Function to display debug information about host and file
void displayDebugHostFileInfo(const char *host, const char *file) {
    printf("Host: %s\n", host);
    printf("File: %s\n", file);
}

// Function to display debug information about address details
void displayDebugAddressInfo(const struct addrinfo *serverAddr) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)serverAddr->ai_addr;
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(serverAddr->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);

    printf("Address Family: %d\n", serverAddr->ai_family);
    printf("Socket Type: %d\n", serverAddr->ai_socktype);
    printf("Protocol: %d\n", serverAddr->ai_protocol);
    printf("Flags: %d\n", serverAddr->ai_flags);
    printf("IP Address: %s\n", ipstr);
}

// Function to display debug information about socket creation
void displayDebugSocketCreation(int sockfd) {
    printf("Socket Descriptor: %d\n", sockfd);
}

// Function to display debug information about the successful RRQ packet transmission
void displayDebugRRQSuccess() {
    printf("RRQ packet sent successfully.\n");
}

// Function to display debug information about the successful WRQ packet transmission
void displayDebugWRQSuccess() {
    printf("WRQ packet sent successfully.\n");
}


// -------------------- Main -------------------- //
int main(int argc, char *argv[]) {
    char *host;
    char *file;

    // Parse command line arguments
    parseCmdArgs(argc, argv, &host, &file);

    // Get server address information using getaddrinfo
    struct addrinfo *serverAddr = getAddressInfo(host, "69");

    // Create and reserve a socket for connection to the server
    int sockfd = createSocket(serverAddr);

    // Send a RRQ (Read Request) to the server
    char *rrqPacket = sendRRQ(sockfd, serverAddr->ai_addr, file);

    // Send a WRQ (Write Request) to the server
    char *wrqPacket = sendWRQ(sockfd, serverAddr->ai_addr, file);

    // Cleanup before exiting the program
    cleanup(sockfd, serverAddr, rrqPacket, wrqPacket);

    // Exit the program successfully
    return EXIT_SUCCESS;
}
