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

        if size_str[:1] != b'\x05':
            response = b''
            response_size = int(size_str)
            while len(response) < response_size:
                chunk, _ = self.client_socket.recvfrom(self.buffer_size)
                response += chunk
            return response.decode()
        else:
            error_msg = size_str[1:].decode()
            print("Error:", error_msg)
            return None



    def list_files(self, dir_path=None):
        if dir_path is None:
            command = chr(0x1)
        else:
            command = chr(0x1) + dir_path

        response = self.send_command(command)
        print("List of files on server: ")
        print(response)

    def help_menu(self):
        print("\nAvailable commands:")
        print("1. LIST - List files on server")
        print("2. GET - Get files from server")
        print("3. PUT - Upload files to server")
        print("4. QUIT - Exit the client\n")

    def receive_file(self, filename, total_packets):
        with open(filename, 'ab') as f:
            packet_number = 1
            while packet_number <= total_packets:
                data, _ = self.client_socket.recvfrom(self.buffer_size)
                if not data:
                    break
                f.write(data)
                print(f"Received packet {packet_number}")
                ack_packet = bytes([0x4, packet_number])
                self.client_socket.sendto(ack_packet, (self.server_address, self.server_port))
                print(f"Sent ACK for packet {packet_number}")
                packet_number += 1

    def get_files(self, filename):
        get_packet = bytes([0x2]) + filename.encode()

        self.client_socket.sendto(get_packet, (self.server_address, self.server_port))

        response, _ = self.client_socket.recvfrom(self.buffer_size)

        if response[0] == 0x5:
            error_msg = response[1:].decode()
            print("Error:", error_msg)
            return
        else:
            total_packets = int(response.decode())

        self.receive_file(filename, total_packets)
        print(f"File received: {filename}")

    def put_files(self, filename):
        print("Not available yet!")

    def cd(self, dir_path):
        command = bytes([0x6]) + dir_path.encode()

        self.client_socket.sendto(command, (self.server_address, self.server_port))

        response, _ = self.client_socket.recvfrom(self.buffer_size)

        if response[0] == 0x5:
            error_msg = response[1:].decode()
            print("Error:", error_msg)
        elif response[0] == 0x4:
            ack_msg = response[1:].decode()
            print("Success:", ack_msg)

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
            dir_path = user_input[1] if len(user_input) > 1 else None
            client.list_files(dir_path)
        elif command == "HELP":
            client.help_menu()
        elif command == "GET":
            if len(user_input) > 1:
                client.get_files(user_input[1])
            else:
                print("Usage: GET <filename>")
        elif command == "PUT":
            if len(user_input) > 1:
                client.put_files(user_input[1])
            else:
                print("Usage: PUT <filename>")
        elif command == "CD":
            if len(user_input) > 1:
                client.cd(user_input[1])
            else:
                print("Usage: cd <dir>")
        elif command == "QUIT":
            client.quit()
            break
        else:
            print("Invalid command!")

if __name__ == "__main__":
    main()
