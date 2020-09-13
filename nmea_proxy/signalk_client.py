#!/usr/bin/env python3

import asyncio
import websockets
import json
import math

class SignalKClient(object):
    data = dict()

    async def get_wind_data(uri):
        async with websockets.connect(uri) as ws:
            while True:
                data = json.loads(await ws.recv())
                if "updates" not in data:
                    continue
                for u in data["updates"]:
                    if "source" not in u:
                        continue
                    if "pgn" not in u["source"]:
                        continue
                    if u["source"]["pgn"] in (130306, 128267):
                        for v in u["values"]:
                            SignalKClient.data[v["path"]] = v["value"]

    async def print_data():
        while True:
            if "environment.wind.angleApparent" in SignalKClient.data:
                print("PW{:1.4}\n".format(-SignalKClient.data["environment.wind.angleApparent"]))
            await asyncio.sleep(.25)

    async def write_data(ser_out):
        while True:
            if "environment.wind.angleApparent" in SignalKClient.data:
                dat = "PW{:1.4}\n".format(-SignalKClient.data["environment.wind.angleApparent"] * 180.0 / math.pi).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.wind.speedApparent" in SignalKClient.data:
                dat = "PX{:1.4}\n".format(-SignalKClient.data["environment.wind.speedApparent"] * 2.0).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.depth.belowKeel" in SignalKClient.data:
                dat = "PY{:1.4}\n".format(-SignalKClient.data["environment.depth.belowKeel"]).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.water.temperature" in SignalKClient.data:
                dat = "PZ{:1.4}\n".format(-SignalKClient.data["environment.water.temperature"] - 273.0).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
        
            await asyncio.sleep(0.25)

    def print_client(self):
        asyncio.get_event_loop().create_task(SignalKClient.print_data())
        asyncio.get_event_loop().run_until_complete(
            SignalKClient.get_wind_data("ws://10.10.10.1:3000/signalk/v1/stream?subscribe=self"))

    def bridge_client(self, ser_out):
        print("bridge_client started")
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        asyncio.get_event_loop().create_task(SignalKClient.write_data(ser_out))
        asyncio.get_event_loop().run_until_complete(
            SignalKClient.get_wind_data("ws://10.10.10.1:3000/signalk/v1/stream?subscribe=self"))
