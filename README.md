# 2023-2024-Client-TFTP

## Overview
TFTP Client is a client program developed as part of the ENSEA Computer Science course. The program is designed to meet the objectives set by the course, which include developing a TFTP client following RFC specifications and integrating Wireshark captures for network analysis. This client facilitates the exchange of files using the Trivial File Transfer Protocol (TFTP) in compliance with the standards outlined in the RFC documents. 

## Objectives
The primary objectives of this project are as follows:
1. **RFC Compliance:** Develop a TFTP client that strictly adheres to the specifications outlined in the relevant RFC documents, ensuring compatibility with other RFC-compliant servers.
2. **File Exchange:** Enable the seamless exchange of files between the client and TFTP servers, providing a reliable mechanism for transferring data over a network.
3. **Wireshark Integration:** Implement functionality to capture and analyze network traffic using Wireshark. This integration aims to enhance debugging capabilities and provide insights into the TFTP communication process.

## Features
- **RFC-Compliant:** Ensures compatibility with TFTP servers that follow RFC specifications.
- **File Transfer:** Supports the transfer of files between the client and TFTP servers.
- **Wireshark Captures:** Integrates Wireshark captures to facilitate network traffic analysis during TFTP communication.

## Getting Started
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/NightFore/2023-2024-Client-TFTP.git
   ```
2. **Go the right directory and compile the .c file:**
    ```bash
   cd directory_name
   gcc program_name.c -o executeName 
    ```

Make sure to replace :
- "directoryName" by the name of the directory
- "program_name.c" by the corresponding .c file
- "executeName" by the name you want to give to the executable code

3. **Run the code:**
    ```bash
    ./executeName [arguments]
    ```
   
Depending on the question, [arguments] will be replaced by an `host` name and a `file` name.

## License
This project is licensed under the [MIT License](LICENSE).