// TP2_4_gettftp.c

// -------------------- Header -------------------- //
// Libraries
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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

// Structure definition for ACK packet
struct ACKPacket {
    uint16_t opcode;                // Operation code
    uint16_t blockNumber;           // Block number
};

// Helper Functions
void handle_error(const char *location, const char *message, const char *perror_message);
void cleanup(struct addrinfo *serverAddr, int sockfd, char *rrqPacket);

// Core Functions
void parseCmdArgs(int argc, char *argv[], char **host, char **file);
struct addrinfo* getAddressInfo(const char *host, const char *port);
int createSocket(const struct addrinfo *serverAddr);
char* sendRRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename);
void receiveFile(int sockfd, const struct sockaddr *serverAddr, const char *filename);
void sendACK(int sockfd, const struct sockaddr *serverAddr, uint16_t blockNumber);

// Debug Functions
void displayDebugHostFileInfo(const char *host, const char *file);
void displayDebugAddressInfo(const struct addrinfo *serverAddr);
void displayDebugSocketCreation(int sockfd);
void displayDebugRRQSuccess();
void displayDebugReceivedDAT(const char *dataPacket, ssize_t bytesRead);
void debugDisplayACKSuccess();



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

// Function to send an ACK packet
void sendACK(int sockfd, const struct sockaddr *serverAddr, uint16_t blockNumber) {
    // Create an ACK packet structure
    struct ACKPacket ackPacket;
    
    // Set the opcode for ACK (4 for ACK) and convert to network byte order
    ackPacket.opcode = htons(4);

    // Set the block number and convert to network byte order
    ackPacket.blockNumber = htons(blockNumber);

    // Send the ACK packet to the server
    ssize_t bytesSent = sendto(sockfd, &ackPacket, sizeof(struct ACKPacket), SENDTO_FLAGS, serverAddr, sizeof(struct sockaddr));

    // Check if the sendto operation was successful
    if (bytesSent == -1) {
        handle_error("sendACK", "Failed to send ACK packet to the server", "sendto");
    }

    debugDisplayACKSuccess();
}

// Function to perform cleanup before exiting the program
void cleanup(struct addrinfo *serverAddr, int sockfd, char *rrqPacket) {
    // Free the linked list of address info
    if (serverAddr != NULL) {
        freeaddrinfo(serverAddr);
    }

    // Close the socket
    if (sockfd != -1) {
        close(sockfd);
    }

    // Free the allocated memory for the RRQ packet
    if (rrqPacket != NULL) {
        free(rrqPacket);
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

    // Set hints for address family (IPv4), socket type (any), protocol (any), and no special flags
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
    // Format of a RRQ packet: opcode (2 bytes) + filename (variable) + \0 (1 byte) + mode (variable) + \0 (1 byte)
    // Minimum size: 9 bytes (excluding filename and mode)

    // Calculate the size of the RRQ packet
    size_t packetSize = sizeof(uint16_t) + strlen(filename) + 1 + strlen(MODE_STRING) + 1;

    // Allocate memory for the RRQ packet
    char *rrqPacket = (char *) malloc(packetSize);
    if (rrqPacket == NULL) {
        handle_error("sendRRQ", "Failed to allocate memory for RRQ packet", "malloc");
    }

    // 1. Set the opcode for Read Request (RRQ) in the RRQ packet
    rrqPacket[0] = 0;
    rrqPacket[1] = RRQ_OPCODE_READ;

    // 2. Copy the filename to the packet
    strcpy(rrqPacket + sizeof(uint16_t), filename);

    // 3. Add a null byte after the filename
    rrqPacket[sizeof(uint16_t) + strlen(filename)] = '\0';

    // 4. Copy the file transfer mode ("octet") to the RRQ packet
    strcpy(rrqPacket + sizeof(uint16_t) + strlen(filename) + 1, MODE_STRING);

    // 5. Add a null byte after the mode
    rrqPacket[sizeof(uint16_t) + strlen(filename) + 1 + strlen(MODE_STRING)] = '\0';

    // Send the RRQ packet to the server
    ssize_t bytesSent = sendto(sockfd, rrqPacket, packetSize, SENDTO_FLAGS, serverAddr, sizeof(struct sockaddr));
    if (bytesSent == -1) {
        free(rrqPacket);
        handle_error("sendRRQ", "Failed to send RRQ packet to the server", "sendto");
    }

    // Display a success message for the RRQ packet transmission
    displayDebugRRQSuccess();

    return rrqPacket;
}

// Function to receive a file (multiple DATA packets) from the server
void receiveFile(int sockfd, const struct sockaddr *serverAddr, const char *filename) {
    // Format of a DATA packet: opcode (2 bytes) + block number (2 bytes) + data (variable)
    // Minimum size: 4 bytes (excluding data)

    // Initialize the block number for the first packet
    uint16_t blockNumber = 1;

    // Continuously receive and acknowledge packets until the transfer is complete
    while (1) {
        // Buffer for receiving the DATA packet
        char dataPacket[BUFSIZ];

        // Receive the DATA packet from the server
        ssize_t bytesRead = recv(sockfd, dataPacket, BUFSIZ, RECV_FLAGS);
        if (bytesRead == -1) {
            handle_error("receiveFile", "Failed to receive DATA packet from the server", "recv");
        }

        // Display debug information about received DATA packet
        displayDebugReceivedDAT(dataPacket, bytesRead);

        // Send the corresponding ACK
        sendACK(sockfd, serverAddr, blockNumber);

        // Increment the block number for the next packet
        blockNumber++;

        // Check if this is the last packet
        if (bytesRead < BUFSIZ) {
            break;
        }
    }
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

// Function to display debug information about socket creation
void displayDebugSocketCreation(int sockfd) {
    printf("----- createSocket -----\n");
    printf("Socket Descriptor: %d\n", sockfd);
    printf("\n");
}

// Function to display debug information about the successful RRQ packet transmission
void displayDebugRRQSuccess() {
    printf("----- sendRRQ -----\n");
    printf("RRQ packet sent successfully.\n");
    printf("\n");
}

// Function to display debug information about received DATA packet
void displayDebugReceivedDAT(const char *dataPacket, ssize_t bytesRead) {
    // Display received data
    printf("----- receiveFile -----\n");
    printf("Received Data (length: %zd bytes): ", bytesRead - sizeof(uint16_t) * 2);
    fwrite(dataPacket + sizeof(uint16_t) * 2, 1, bytesRead - sizeof(uint16_t) * 2, stdout);
    printf("\n\n");
}

// Function to display a debug success message for ACK packet transmission
void debugDisplayACKSuccess() {
    printf("----- sendACK -----\n");
    printf("ACK packet sent successfully.\n");
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

    // Create and reserve a socket for connection to the server
    int sockfd = createSocket(serverAddr);

    // Send a RRQ (Read Request) to the server
    char *rrqPacket = sendRRQ(sockfd, serverAddr->ai_addr, file);

    // Receive the file (single DATA packet) from the server
    receiveFile(sockfd, serverAddr->ai_addr, file);

    // Cleanup before exiting the program
    cleanup(serverAddr, sockfd, rrqPacket);

    // Exit the program successfully
    return EXIT_SUCCESS;
}
