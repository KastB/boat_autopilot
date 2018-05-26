"""Script for Tkinter GUI chat client."""
from socket import AF_INET, socket, SOCK_STREAM
from threading import Thread
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
from nmea_proxy.decode_raw_data import decode_data

graph_data = []
run = True


def receive():
    """Handles receiving of messages."""
    while run:
        try:
            graph_data.append(client_socket.recv(BUFSIZ).decode("ASCII"))
        except OSError:  # Possibly client has left the chat.
            break

def animate(i):
    xs = []
    data_points = []
    if True:
        data_points = ["magMin[0]",
                       "magMin[1]",
                       "magMin[2]",
                       "magMax[0]",
                       "magMax[1]",
                       "magMax[2]"]
    if True:
        data_points = ["yaw",
                       "pitch",
                       "roll"]
    ys = []
    for i in data_points:
        ys.append([])

    for line in graph_data:
        if len(line) > 1:
            data = decode_data(line)
            try:
                x = float(data["Millis"])
                for dp in range(len(data_points)):
                    ys[dp].append(float(data[data_points[dp]]))
            except Exception as e:
                continue
            xs.append(x)
    ax1.clear()
    for y in ys:
        ax1.plot(xs, y)

# ----Now comes the sockets part----
HOST = "127.0.0.1"
PORT = 2948

BUFSIZ = 1024
ADDR = (HOST, PORT)

client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.connect(ADDR)

receive_thread = Thread(target=receive)
receive_thread.start()


style.use('fivethirtyeight')

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

ani = animation.FuncAnimation(fig, animate, interval=100)
plt.show()

run = False
