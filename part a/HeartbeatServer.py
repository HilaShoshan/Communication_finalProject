from socket import *
import time


# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(('', 12000))

while True:  # אם עברו 30 שניות ולא קיבלנו הודעה צריך להניח שהקליינט נעצר...

    # Receive the client packet along with the address it is coming from
    message, address = serverSocket.recvfrom(1024)



