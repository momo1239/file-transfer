# Design Plan

## Project Directory Tree
Maybe will look something like this

file/
		client/
					buffclient.py
		server/
					main.c
		protocol/
						protocol.c
						protocol.h
		Makefile

### main.c
main.c will have main code for the server. check cli args for port number. create socket bind socket communication loop

### protocol.c and protocol.h
protocol.c will have function to handle client requests and send responses back to client. 

### packet structure
opcodes
0x1 = DIR Packet
0x2 = get packet
0x3 = put packet
0x4 = ack packet
0x5 = error packet
0x6 = data packet

dir packet will have an opcode 0x1 

get and put packets will have their opcode respectively and then the filename to get or put. it will also need to have a number or something to distinguish text mode or ascii mode (potentially)

ack packets will contain their opcode and the packet #

error packet will contain their opcode and the error message. for example, if file does not exist (to get) 

data packet will have their opcode, the packet #, and then the data to transfer

### random theorizing below

DIR -
1. client sends 0x1 to server
2. server receives 0x1 and uses readdir to read files in currently dir into a buffer
3. checks size of buffer and sends it to client
4. client recvfrom size of the buffer (dir listing)
5. server sends dir listing

GET (maybe?) -
6. client sends 0x2 and filename to server
7. server receives 0x2 and filename
8. parses out filename
9. server checks if file exists
10. if not exist return error message to client / or timeout? not sure
11. if exist opens the file and read contents from the file
12. write into buffer then send buffer to client
13. MAYBE? calculate how many packets will be sent to client. send that to client first. put numbering on the buffer to send to client and have the client send an ack packet number opcode to confirm they received the packet(??? big maybe on this)
14. continue sending until every buffer has been sent
15. client receives all the buffer creates a new file with filename it requested and writes these buffers in order into that file.
(huge question mark on all of this)

### buffclient.py
Create a class for client. have it initialize socket, addr, ports and all that. 
Create functions for each commands.
Additional create a function that will send commands over the socket and receive responses.

Create a function for a help menu to list available commands

Create a function to quit and exit the client.

In main function: check for command line args for srv addr and srv port 

while loop to receive user input for commands
