import socket
import time
from threading import Thread


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_addr = ('localhost', 12000)

heartbeat_msg = "~~~~~~ I'm Alive ~~~~~~"

try:

    def background_task():
        while not background_task.cancelled:
            sock.sendto(heartbeat_msg.encode(), server_addr)
            time.sleep(30)

    background_task.cancelled = False

    t = Thread(target=background_task)
    t.start()

    background_task.cancelled = True

except socket.timeout:
    print("Requested Time out\n")


