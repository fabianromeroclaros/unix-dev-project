# FINAL PROJECT

Fabián Romero Claros

## Instructions

### 1. Compile the Program

Compile the server program using GCC:

```bash
gcc -o final_project final_project.c -pthread
```

### 2. Run the Program

Run the compiled server program, specifying the port number on which the server should listen:

```bash
./final_project <port_number>
```

Replace `<port_number>` with the desired port number (e.g., 8080).

### 3. Connecting to the Server

Once the server is running, you can connect to it using a client program such as `telnet` or `netcat`. For example:

```bash
nc <ip> <port_number>
```

Replace `<ip>` with your ip and  `<port_number>` with the same port number used when starting the server.

### 4. Shutting Down the Server

To shut down the server gracefully, press `Ctrl + C` in the terminal where the server is running. This will handle the `SIGINT` signal and terminate the server program.
