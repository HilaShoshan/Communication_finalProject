# UDPPingerServer.py
# We will need the following module to generate randomized lost packets

import random
from socket import *
import time


# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(('', 12000))

last_packet = 0  # saves the number of the last packet -- to pay attention to losses

while True:

    # Generate random number in the range of 0 to 10
    rand = random.randint(0, 10)

    # Receive the client packet along with the address it is coming from
    message, address = serverSocket.recvfrom(1024)
    # Capitalize the message from the client
    message = message.upper()

    # Calculate the time difference - to check the speed of service
    now = time.time()
    # split the massage to get the time
    split_arr = message.split(' ', 2)  # maxsplit = 2 (first 3 parts)
    time_str = split_arr[2]  # the time in ctime format
    msg_time = time.strptime(time_str)
    differ = now - msg_time
    print("The different is : ", differ)

    # Report any lost packets - to check the reliability
    curr_packet = int(split_arr[1])
    if curr_packet - last_packet > 1:  # one or more packets are lost
        for i in range(last_packet+1, curr_packet):
            print("packet ", i, " lost!")
    last_packet = curr_packet

    # If rand is less is than 4, we consider the packet lost and do not respond
    if rand < 4:
        continue

    # Otherwise, the server responds
    serverSocket.sendto(message, address)


