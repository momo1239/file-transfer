import socket
import sys

class TransferClient:
    def __init__(self, server_address, server_port):
        self.server_address = server_address
        self.server_port = server_port
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.buffer_size = 1024

    def send_command(self, command):
            self.client_socket.sendto(command.encode(), (self.server_address, self.server_port))
            size_str, _ = self.client_socket.recvfrom(self.buffer_size)
            response_size = int(size_str)
            response = b''

            while len(response) < response_size:
                chunk, _ = self.client_socket.recvfrom(self.buffer_size)
                response += chunk

            return response.decode()

    def list_files(self):
            response = self.send_command(chr(0x1))
            print("List of files on server: ")
            print(response)

    def help_menu(self):
            print("\nAvailable commands:")
            print("1. LIST - List files on server")
            print("2. GET - Get files from server")
            print("3. PUT - Upload files to server")
            print("4. QUIT - Exit the client\n")

    def get_files(self, filename):
        print("Not available yet!")

    def put_files(self, filename):
        print("Not available yet!")

        
    def quit(self):
            self.client_socket.close()
            print("Exiting...")

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 client.py <server_address> <server_port>")
        sys.exit(1)

    server_address = sys.argv[1]
    server_port = int(sys.argv[2])

    client = TransferClient(server_address, server_port)

    print("Type HELP for available commands")

    while True:
        user_input = input("buff> ").strip().split(" ", 2)
        command = user_input[0].upper()

        if command == "LIST":
            client.list_files()
        elif command == "HELP":
            client.help_menu()
        elif command == "GET":
            client.get_files("empty")
        elif command == "PUT":
            client.put_files("empty")
        elif command == "QUIT":
            client.quit()
            break
        else:
            print("Invalid command!")

if __name__ == "__main__":
    main()


