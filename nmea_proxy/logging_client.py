"""Script for Tkinter GUI chat client."""
from socket import AF_INET, socket, SOCK_STREAM
from pathlib import Path
import datetime

now = datetime.datetime.now()
home = str(Path.home())


# ----Now comes the sockets part----
# HOST = "127.0.0.1"
HOST = "10.10.10.1"
PORT = 2948

BUFSIZ = 1024
ADDR = (HOST, PORT)

client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.connect(ADDR)


with open(home + "/data/autopilot{}.log".format(now.strftime("%Y-%m-%d")), "a") as fh:
    try:
        while True:
            data = client_socket.recv(BUFSIZ).decode("ASCII")
            if len(data) == 0:
                client_socket = socket(AF_INET, SOCK_STREAM)
                client_socket.connect(ADDR)

            fh.write(data)
    except (OSError,KeyboardInterrupt) as e:
        print(e)
        pass
