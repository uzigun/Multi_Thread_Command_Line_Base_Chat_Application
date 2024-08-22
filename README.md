# Multi Thread Command Line Based Chat Application

This project consists of a server and client application implemented in C using Winsock for Windows. The server handles multiple client connections and broadcasts messages to all connected clients. The client application allows users to send and receive messages.

## Features

- **Server:**
  - Handles up to 10 simultaneous clients.
  - Uses threads to manage each client connection.
  - Broadcasts received messages to all other connected clients.
  - Utilizes critical sections to manage concurrent access to the client list.

- **Client:**
  - Connects to the server and sends user input messages.
  - Receives and displays messages from the server.
  - Runs a separate thread for handling user input and sending messages.

## Requirements

- Windows operating system
- Visual Studio or any compatible C compiler
- Winsock library (ws2_32.lib)

## Compilation and Execution

### Server

1. Open the server source code in your C compiler.
2. Compile the code. Ensure that you link against the Winsock library (`ws2_32.lib`).
```bash
gcc -o server.exe server.c -lws2_32
```
3. Run the compiled server executable.
### Client

1. Open the client source code in your C compiler.
2. Compile the code. Ensure that you link against the Winsock library (`ws2_32.lib`).
```bash
gcc -o client.exe client.c -lws2_32
```
3. Run the compiled client executable. Ensure that it connects to the correct server IP address and port.
## Usage

1. **Start the Server:**
   - Run the server application. It will start listening on port 8000.

2. **Connect Clients:**
   - Run one or more client applications. They will connect to the server and allow you to send and receive messages.

3. **Send and Receive Messages:**
   - Type messages into the client console to send them to the server.
   - Messages from other clients will be displayed in the client console.

## Code Overview

### Server Code

- **`main()`**: Initializes Winsock, creates and binds the server socket, listens for incoming connections, and manages client threads.
- **`ClientHandler()`**: Handles communication with a connected client, including receiving messages and broadcasting them to other clients.

### Client Code

- **`main()`**: Initializes Winsock, creates the client socket, connects to the server, and starts a thread for handling user input.
- **`InputHandler()`**: Reads user input from the console and sends it to the server.

## Troubleshooting

- Ensure that the server and client applications are running on the same network or use the correct IP address.
- Verify that port 8000 is not blocked by a firewall.
- Make sure the Winsock library is properly linked during compilation.
## Acknowledgements

- Microsoft for the Winsock library.
- C Programming resources and documentation.
