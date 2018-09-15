"""Script for Tkinter GUI chat client."""
from socket import AF_INET, socket, SOCK_STREAM
from threading import Thread
import matplotlib.pyplot as plt
from matplotlib.axes import Axes
import matplotlib.animation as animation
from matplotlib import style
from nmea_proxy.decode_raw_data import decode_data
import time

graph_data = []

global connected
global run
run = True
connected = False

def receive():
    """Handles receiving of messages."""
    global run
    while run:
        try:
            line = client_socket.recv(BUFSIZ).decode("ASCII")
            if len(line) == 0:
                connected = False
                time.sleep(1)

            if len(line) > 1:
                data = decode_data(line)
            graph_data.append(data)
        except OSError:  # Possibly client has left the chat.
            break


def animate(i):
    xs = []
    data_points = []
    if False:
        data_points = ["magMin[0]",
                       "magMin[1]",
                       "magMin[2]",
                       "magMax[0]",
                       "magMax[1]",
                       "magMax[2]"]
    if False:
        data_points = ["yaw",
                       "pitch",
                       "roll"]
    if False:
        data_points = ["freq"]

    if False:
        data_points = ["m_speed.tripMileage",
                       "m_speed.totalMileage"]

    if False:
        data_points = ["m_depth.defective",
                       "m_depth.depthBelowTransductor",
                       "m_depth.metricUnits",
                       "m_depth.unknown"]
    if False:
        data_points = ["m_wind.apparentAngle"]

    if False:
        data_points = ["TargetPosition",
                       "diagA",
                       "diagB"]


    ys = []
    for i in data_points:
        ys.append([])

    for data in graph_data:
        try:
            x = float(data["Millis"])
            for dp in range(len(data_points)):
                ys[dp].append(float(data[data_points[dp]]))
        except Exception as e:
            continue
        xs.append(x)
    ax1.clear()
    for y in range(len(ys)):
        ax1.plot(xs, ys[y], label=data_points[y])
    ax1.legend()
    # ax1.set_ylim(bottom=-10, top=10)

# ----Now comes the sockets part----
HOST = "127.0.0.1"
PORT = 2948

BUFSIZ = 1024
ADDR = (HOST, PORT)

client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.connect(ADDR)
connected = True

receive_thread = Thread(target=receive)
receive_thread.start()


# style.use('fivethirtyeight')

fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

ani = animation.FuncAnimation(fig, animate, interval=100)
plt.show()


run = False
