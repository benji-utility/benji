import socket

host = "127.0.0.1"
port = int(input("port: "))

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.connect((host, port))

try:
    message = "cpu_all;gpu_all;ram_all;"

    print(f"Sending: {message}")
    server_socket.send(message.encode())
    print("Sent")

    print("\nWaiting for response...")
    response = server_socket.recv(1024)
    print(f"Server response: {response.decode()}")

finally:
    server_socket.close()
