import socket
import sys

#HOST, PORT = "localhost", 9999
HOST, PORT = "localhost", int(sys.argv[1])
#data = " ".join(sys.argv[1:])
data = 'hello yangtao'

# Create a socket (SOCK_STREAM means a TCP socket)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(data + "\n")

    # Receive data from the server and shut down
#    received = sock.recv(1024)
finally:
    sock.close()

#print "Sent:     {}".format(data)
#print "Received: {}".format(received)