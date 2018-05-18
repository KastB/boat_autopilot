"""
Description

@author: Bernd Kast
@copyright: Copyright (c) 2018, Siemens AG
@note:  All rights reserved.
"""
import socket
import threading
import time


class UDPServer(object):
    def __init__(self, ip, port):
        self.port = port
        self.ip = ip

        self.sock = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_DGRAM)  # UDP
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, True)
        self.sock.settimeout(5)

    def write(self, msg):
        self.sock.sendto(msg.encode("utf-8"), ("10.42.0.0", self.port))


class TCPServer(object):
    def __init__(self, ip, port, max_connections=20):
        self.buffer_size = 1024
        self.port = port
        self.ip = ip

        self.sock = socket.socket(socket.AF_INET,  # Internet
                                  socket.SOCK_STREAM)  # TCP
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind((ip, port))
        self.sock.listen(max_connections)

        self.lock = True
        self.clients = []

        self.client_listener = threading.Thread(
            target=self.listen_for_clients,
        )
        self.client_listener.start()

    def listen_for_clients(self):
        while(1):
            client, address = self.sock.accept()
            self.aquire_lock()
            self.clients.append(client)
            self.release_lock()

    def write(self, msg):
        self.aquire_lock()
        clients = list(self.clients)
        self.release_lock()
        for c in clients:
            try:
                c.send(msg.encode())
            except Exception as e:
                print(e)
                self.clients.remove(c)
        self.release_lock()

    def connection_handler(self, socket):
        request = socket.recv(self.buffer_size)

        # TODO: handle this as well
        print('Received {}'.format(request))

    def stop(self):
        self.aquire_lock()
        for c in self.clients:
            c.close()

    def aquire_lock(self):
        while not self.lock:
            time.sleep(0.1)
        self.lock = False

    def release_lock(self):
        self.lock = True