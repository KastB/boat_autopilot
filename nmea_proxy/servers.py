"""
Description

@author: Bernd Kast
"""
import socket
import threading
import time


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
        self.run = True

        self.sock = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_STREAM)  # TCP
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((ip, port))
        self.sock.listen(max_connections)
        self.sock.settimeout(1)

        self.lock = True
        self.clients = []
        self.out_buffer = []

        self.client_listener = threading.Thread(
            target=self.listen_for_clients,
        )
        self.client_listener.start()

    def listen_for_clients(self):
        while self.run:
            try:
                client, address = self.sock.accept()
            except socket.timeout:
                continue
            self.acquire_lock()
            self.clients.append(client)
            self.release_lock()
            threading.Thread(target=self.connection_handler, args=(client,)).start()

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

    def connection_handler(self, client):
        while self.run:
            msg = client.recv(self.buffer_size).decode("ASCII")
            if msg == "":
                break
            self.acquire_lock()
            self.out_buffer.append(msg)
            self.release_lock()

    def get_out_buffer(self):
        self.acquire_lock()
        out = self.out_buffer
        self.out_buffer = []
        self.release_lock()
        return out

    def close(self):
        self.run = False
        self.acquire_lock()
        for c in self.clients:
            c.close()

    def acquire_lock(self):
        while not self.lock:
            time.sleep(0.1)
        self.lock = False

    def release_lock(self):
        self.lock = True
