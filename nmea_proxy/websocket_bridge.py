"""Script for Tkinter GUI chat client."""
from socket import AF_INET, socket, SOCK_STREAM
from pathlib import Path
import datetime
import asyncio
import websockets
from decode_raw_data import raw2json
import json
# ----Now comes the sockets part----
HOST = "127.0.0.1"
# HOST = "10.10.10.1"
PORT = 2948

websocket_clients = list()
# create handler for each connection
async def handler(websocket, path):
    try:
        websocket_clients.append(websocket)
        while True:
            data = json.loads(await websocket.recv())
            # send command to tcp
            for cmd in data["cmds"]:
                writer.write(cmd.encode())
    except:
        return

async def send_dat():
    reader, writer = await asyncio.open_connection(HOST, PORT)     
    print("TCP opened")
    while True:
        try:
            data = await reader.readline()
            dat = raw2json(data.decode("ASCII"))
            da = json.dumps(dat).encode()
            if not dat:
                continue
            for websocket in list(websocket_clients):
                try:
                    await websocket.send(da)
                except Exception as e:
                    websocket_clients.remove(websocket)
                    print(f"Send_dat1: {e}" )
        except Exception as e:
            reader, writer = await asyncio.open_connection(HOST, PORT) 
            print(f"Send_dat2: {e}" )
start_server = websockets.serve(handler, "", 8000)
loop =asyncio.get_event_loop()
loop.run_until_complete(start_server)
loop.create_task(send_dat())
loop.run_forever()

    