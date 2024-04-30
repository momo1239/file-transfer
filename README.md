# file-transfer project
Implement a custom protocol for file transfer over UDP in C and Python. 

# Compiling and Usage
Project comes with a make file. 
Run "make" and "make clean"

# Project Structure
```
buff |--download/      Contains sample files for testing purposes
     |--build/         Contains executable files after linking
     |--src/           Contains source code files (.c)
     |--Makefile       Project Makefile
```

# Barebones, Ugly, Reinventing the Wheel, Strangely Implemented: Custom Protocol
This protocol is implemented over UDP and supports listing files and directories, changing directories, and downloading/uploading files. There are 6 types of packets.

```
Opcode - Packet
0x1 - DIR
0x2 - GET
0x3 - PUT
0x4 - ACK
0x5 - ERR
0x6 - CD
No Opcode - DATA
```
A file transfer begins with the client sending a GET or PUT packet which also requests a connection to the server. The file is sent in 1024 byte packet chunks and each chunk must be acknowledged by the client before the next packet can be sent.

## Packets

GET and PUT packets will have the following format:
```
+--------+------------+
| Opcode |  Filename  |
+--------+------------+
```

ACK Packets:
```
+--------+------------+
| Opcode |   Packet # |
+--------+------------+
```

Error Packets:
```
+--------+------------+
| Opcode |  ErrorMsgg | 
+--------+------------+
```
