# 2023-2024-Client-TFTP

## Overview
TFTP Client is a client program developed as part of the ENSEA Computer Science course. The program is designed to meet the objectives set by the course, which include developing a TFTP client following RFC specifications. The communication occurs over the User Datagram Protocol (UDP).

## Features

- **RFC-Compliant:** Ensures compatibility with TFTP servers that follow RFC specifications.
- **File Transfer:** Supports the transfer of files between the client and TFTP servers.

## Getting Started

### Prerequisites

- The program is written in C and requires a C compiler (e.g., GCC) to build.

### Building the TFTP Client

```bash
gcc program_name.c -o tftp_client
```
*Replace "program_name.c" with the specific code file from any question's folder.*

### Running the TFTP Client

To run the TFTP Client, use the following command:

```bash
./tftp_client host file get/put
```

- Replace `host` with the TFTP server's address.
- Replace `file` with the name of the file you want to transfer.
- Choose either `get` to download a file from the server or `put` to upload a file to the server.

## Usage

### 1. Get File

To download a file from the TFTP server and obtain server information, use the following command:

```bash
./tftp_client host file get
```

- Replace `host` with the TFTP server's address.
- Replace `file` with the name of the file you want to download.

Example:

```bash
./tftp_client srvtpinfo1.ensea.fr test.txt get
```

### 2. Put File

To upload a file to the TFTP server and obtain server information, use the following command:

```bash
./tftp_client host file put
```

- Replace `host` with the TFTP server's address.
- Replace `file` with the name of the file you want to upload.

Example:

```bash
./tftp_client srvtpinfo1.ensea.fr test.txt put
```

## Code Structure

### Header Files

- `<arpa/inet.h>`, `<netdb.h>`, `<stdio.h>`, `<string.h>`, `<sys/socket.h>`, `<sys/types.h>`, `<unistd.h>`: Standard C libraries providing essential functionalities for networking and I/O operations.

### Constants

- `TFTP_SERVER_PORT`: Port number for the TFTP server (default value: 69).
- `BLOCK_SIZE`: Block size for data packets (default value: 1024 bytes).

### Helper Functions

- `processUserInput(int sockfd, struct addrinfo *serverAddr, const char *action, const char *file)`: Handles user input for initiating "get" (download) or "put" (upload) operations.
- `handle_error(const char *location, const char *message, const char *perror_message)`: Standardized error handling function with informative messages.
- `cleanup(struct addrinfo *serverAddr, int sockfd, char *rrqPacket, char *wrqPacket)`: Cleanup function to release allocated resources.

### Core Functions

- `parseCmdArgs(int argc, char *argv[], char **host, char **file, char **action)`: Parses command-line arguments, extracting host address, file name, and operation type (get/put).
- `getAddressInfo(const char *host, const char *port)`: Obtains address information for the TFTP server using `getaddrinfo`.
- `createSocket(const struct addrinfo *serverAddr)`: Creates a UDP socket and establishes a connection to the TFTP server.
- `sendRRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename)`: Sends a Read Request (RRQ) packet to the server.
- `receiveFile(int sockfd, const struct sockaddr *serverAddr, const char *filename)`: Receives a file from the TFTP server.
- `sendACK(int sockfd, const struct sockaddr *serverAddr, uint16_t blockNumber)`: Sends an acknowledgment (ACK) packet to the server.
- `sendWRQ(int sockfd, const struct sockaddr *serverAddr, const char *filename)`: Sends a Write Request (WRQ) packet to the server.
- `sendFile(int sockfd, const struct sockaddr *serverAddr, const char *file)`: Sends a file to the TFTP server.

### Debug Functions

- `displayDebugHostFileInfo(const char *host, const char *file)`: Displays debug information about the host and file.
- `displayDebugAddressInfo(const struct addrinfo *serverAddr)`: Displays debug information about the server address.
- `displayDebugSocketCreation(int sockfd)`: Displays debug information about socket creation.
- `displayDebugRRQSuccess()`: Displays debug information about successful RRQ packet transmission.
- `displayDebugReceivedDAT(const char *dataPacket, ssize_t bytesRead)`: Displays debug information about received data packets.
- `debugDisplayACKSuccess()`: Displays debug information about successful acknowledgment.
- `displayDebugWRQSuccess()`: Displays debug information about successful WRQ packet transmission.
- `displayDebugSentDAT(const char *dataPacket, ssize_t bytesSent)`: Displays debug information about sent data packets.
- `displayDebugReceivedACK(const struct ACKPacket *ackPacket)`: Displays debug information about received acknowledgment packets.

## Results

1. **Arguments**

```bash
./TP2_1_cmd_args srvtpinfo1.ensea.fr test.txt
----- parseCmdArgs -----
Host: srvtpinfo1.ensea.fr
File: test.txt
```

This command tests the parsing of command-line arguments in the `parseCmdArgs` function, extracting the host and file information.

2. **Address Info**

```bash
./TP2_2_getaddrinfo srvtpinfo1.ensea.fr test.txt
----- getAddressInfo -----
Address Family: 2
Socket Type: 2
Protocol: 17
Flags: 0
IP Address: xx.xx.xx.xx (Note: IP address has been hidden for privacy)
```

The obtained information from `getAddrInfo` includes the following details:
- **Address Family:** The address family is 2, indicating IPv4.
- **Socket Type:** The socket type is 2, indicating SOCK_DGRAM, which corresponds to UDP.
- **Protocol:** The protocol is 17, indicating the UDP protocol.
- **Flags:** There are no special flags set (value: 0).
- **IP Address:** The IP address.

3. **Socket**

```bash
./TP2_3_socket_connection srvtpinfo1.ensea.fr test.txt
----- createSocket -----
Socket Descriptor: 3
```

This command demonstrates the successful creation of a UDP socket and the establishment of a connection to the TFTP server using the createSocket function.

*Note: The socket descriptor number (in this case, "3") is assigned by the operating system and serves as an identifier for the created socket.*


4. **gettftp**

```bash
./TP2_4_gettftp srvtpinfo1.ensea.fr test.txt
----- sendRRQ -----
RRQ packet sent successfully.

----- receiveFile -----
Sent Data (length: 10 bytes):
HelloWorld

----- sendACK -----
ACK packet sent successfully.
```

This command tests the get operation by initiating a Read Request (RRQ) packet, receiving a file containing the text "HelloWorld," and successfully sending acknowledgment packets to the TFTP server.

5. **puttftp**

```bash
./TP2_5_puttftp srvtpinfo1.ensea.fr test.txt
----- sendWRQ -----
WRQ packet sent successfully.

----- sendFile -----
Sent Data (length: 10 bytes):
HelloWorld

----- sendFile -----
Received ACK:
Opcode: 4
Block Number: 1
```

This command tests the put operation by initiating a Write Request (WRQ) packet, sending a file containing the text "HelloWorld," and successfully receiving acknowledgment packets from the TFTP server. In the received acknowledgment packet, the opcode "4" signifies that it is an acknowledgment packet, and the block number is "1," indicating the acknowledgement for the first block of data sent.
Corrige le texte de la partie "6. **blocksize**" comme suit:

6. **blocksize**

**Receive Operation:**

```bash
./TP2_6_blocksize_option srvtpinfo1.ensea.fr test.txt get
----- sendRRQ -----
RRQ packet sent successfully.

----- receiveFile -----
Sent Data (length: 10 bytes):
HelloWorld

----- sendACK -----
ACK packet sent successfully.
```

This command initiates a Read Request (RRQ) packet and successfully receives a file from the TFTP server, containing the text "HelloWorld".

**Send Operation:**

```bash
./TP2_6_blocksize_option srvtpinfo1.ensea.fr test.txt put
----- sendWRQ -----
WRQ packet sent successfully.

----- sendFile -----
Sent Data (length: 10 bytes):
HelloWrite

----- sendFile -----
Received ACK:
Opcode: 6
Block Number: 25196
```

However, the send operation encounters an issue. An attempt to send an acknowledgment (ACK) packet is made, but an error occurs. The received packet shows an unknown opcode and an unexpected block number.

## Contributing

This project is part of an academic assignment and is not open to external contributions.

## License

This project is licensed under the [MIT License](LICENSE).
