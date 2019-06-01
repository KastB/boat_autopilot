"""
Description

@author: Bernd Kast
"""
import socket
import threading
import time
import os


class UDPServer(object):
    def __init__(self, ip, port, broadcast=False):
        self.port = port
        self.ip = ip

        self.sock = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_DGRAM)  # UDP
        if broadcast:
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, True)
        self.sock.settimeout(5)

    def write(self, msg):
        self.sock.sendto(msg.encode("ASCII"), (self.ip, self.port))

    def close(self):
        pass


class TCPServer(object):
    def __init__(self, ip, port, max_connections=20):
        self.buffer_size = 1024
        self.port = port
        self.ip = ip
        global run
        run = True

        self.sock = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_STREAM)  # TCP
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((ip, port))
        self.sock.listen(max_connections)
        self.timeout = 1
        self.sock.settimeout(self.timeout)

        self.lock = True
        self.out_buffer = []

        self.clients = []

        self.client_listener = threading.Thread(
            target=self.listen_for_clients,
        )
        self.client_listener.start()

        self.client_threads = []

    def listen_for_clients(self):
        global run
        while run:
            try:
                client, address = self.sock.accept()
                print("new client:")
                print(address)
            except socket.timeout:
                continue

            client.settimeout(self.timeout)
            self.acquire_lock()
            self.clients.append(client)
            self.release_lock()
            self.client_threads.append(threading.Thread(target=self.client_handler, args=(client,)))
            self.client_threads[-1].start()

    def write(self, msg):
        self.acquire_lock()
        clients = list(self.clients)
        self.release_lock()
        for c in clients:
            try:
                c.send(msg.encode("ASCII"))
            except Exception as e:
                print(e)
                self.clients.remove(c)
        self.release_lock()

    def client_handler(self, client):
        global run
        while run:
            try:
                msg = client.recv(self.buffer_size).decode("ASCII")
                if msg == "":
                    break
                if msg.startswith("#"):
                    if msg.startswith("#REBOOT"):
                        os.system("sudo reboot")
                    elif msg.startswith("#SHUTDOWN"):
                        os.system("sudo poweroff")

                self.acquire_lock()
                self.out_buffer.append(msg)
                self.release_lock()
            except socket.timeout:
                pass
        client.close()

    def get_out_buffer(self):
        self.acquire_lock()
        out = self.out_buffer
        self.out_buffer = []
        self.release_lock()
        return out

    def close(self):
        global run
        run = False
        self.client_listener.join()
        for ct in self.client_threads:
            ct.join()

    def acquire_lock(self):
        while not self.lock:
            time.sleep(0.1)
        self.lock = False

    def release_lock(self):
        self.lock = True
