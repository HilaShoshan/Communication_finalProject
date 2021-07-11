import socket
import time
import math

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_addr = ('localhost', 12000)
sock.settimeout(1)  # wait up to one second for a reply

try:

    # parameters for the final report:
    minRTT = math.inf
    maxRTT = - math.inf
    sumRTT = 0
    numRTT = 0
    numLosts = 0

    # send 10 pings to the server using UDP (connectionless protocol)
    for i in range(1, 11):
        start = time.time()
        message = 'Ping ' + str(i) + " " + time.ctime(start)
        try:

            sent = sock.sendto(message, server_addr)
            print("Sent " + message)
            data, server = sock.recvfrom(4096)
            print("Received " + data)
            end = time.time()
            RTT = end - start
            print("RTT: " + str(RTT) + " seconds\n")

            if RTT < minRTT:
                minRTT = RTT
            if RTT > maxRTT:
                maxRTT = RTT
            sumRTT += RTT
            numRTT += 1

        except socket.timeout:
            print("Requested Time out\n")
            numLosts += 1

finally:
    print("---------------REPORT-----------------")
    print("closing socket")
    print("Minimum RTT: ", minRTT)
    print("Maximum RTT: ", maxRTT)
    print("Average RTT: ", sumRTT/numRTT)
    print("Packet Loss Rate: ", (numLosts/10)*100)  # in percentage
    sock.close()