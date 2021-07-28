# UDPPingerServer.py
# We will need the following module to generate randomized lost packets

import random
from socket import *
import time

from threading import Timer  # for the Heartbeat protocol


def server():

    # Create a UDP socket
    # Notice the use of SOCK_DGRAM for UDP packets
    serverSocket = socket(AF_INET, SOCK_DGRAM)

    # Assign IP address and port number to socket
    serverSocket.bind(('', 12000))
    counter = 0

    while True:

        # Generate random number in the range of 0 to 10
        rand = random.randint(0, 10)

        # Receive the client packet along with the address it is coming from
        message, address = serverSocket.recvfrom(1024)
        counter += 1
        # Capitalize the message from the client
        message = message.upper()

        # Calculate the time difference (for the Heartbeat)
        now = time.time()
        # split the massage to get the time
        split_arr = message.split(' ', 2)  # maxsplit = 2 (first 3 parts)
        time_str = split_arr[2]  # the time in ctime format
        msg_time = time.strptime(time_str)
        differ = now - msg_time
        # print ("The different is : " , differ )

        # Report any lost packets  ----  how???

        # If the Heartbeat packets are missing for some specified period of time
        # we can assume that the client application has stopped

        # If rand is less is than 4, we consider the packet lost and do not respond
        if rand < 4:
            continue

        # Otherwise, the server responds
        serverSocket.sendto(message, address)


t = Timer(10.0, server)  # wait for a massage for 10 seconds
t.start()  # after 10 seconds, "hello, world" will be printed
t.cancel()  # cancels execution, this only works before the 10 seconds is elapsed

