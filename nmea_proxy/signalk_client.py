#!/usr/bin/env python3

import asyncio
import websockets
import json
import math

class SignalKClient(object):
    data = dict()

    async def get_signalk_data(uri):
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
                    if u["source"]["pgn"] in (130306, 128267, 130311):
                        for v in u["values"]:
                            SignalKClient.data[v["path"]] = v["value"]
                """
                async with websockets.connect("ws://10.10.10.1:3001/signalk/v1/stream") as ws2:
                    ws2.send(json.dumps({
                                            "path": 'steering.autopilot',
                                            "value": { "state": "off" },
                                            # context: 'vessels.urn:mrn:signalk:uuid:a9d2c3b1-611b-4b00-8628-0b89d014ed60',
                                            "source": {
                                                "label": 'autopilot_init',
                                                "type": 'SignalK'
                                            },
                                            '$source': 'SignalKInit',
                                            #timestamp: '2014-08-15T19:00:02.392Z'
                                            }))
                """
                

    async def print_data():
        while True:
            print(list(SignalKClient.data.keys()))
            if "environment.wind.angleApparent" in SignalKClient.data:
                print("PW{:1.4}\n".format(-SignalKClient.data["environment.wind.angleApparent"]))
            if "environment.water.temperature" in SignalKClient.data:
                dat = "PZ{:1.4}\n".format(-SignalKClient.data["environment.water.temperature"] - 273.0).encode("ASCII")
                print(dat)
            await asyncio.sleep(.25)

    async def write_data(ser_out):
        while True:
            if "environment.wind.angleApparent" in SignalKClient.data:
                dat = "PW{:1.4}\n".format(SignalKClient.data["environment.wind.angleApparent"]*180/math.pi).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.wind.speedApparent" in SignalKClient.data:
                dat = "PX{:1.4}\n".format(SignalKClient.data["environment.wind.speedApparent"] * 2.0).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.depth.belowKeel" in SignalKClient.data:
                dat = "PY{:1.4}\n".format(SignalKClient.data["environment.depth.belowKeel"]).encode("ASCII")
                try:
                    ser_out.write(dat)
                    print("write success")
                except Exception as e:
                    print("SignalK Write:")
                    print(e)
            if "environment.water.temperature" in SignalKClient.data:
                dat = "PZ{:1.4}\n".format(
                    SignalKClient.data["environment.water.temperature"] - 273.0).encode("ASCII")
                print(dat)
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
            SignalKClient.get_signalk_data("ws://10.10.10.1:3000/signalk/v1/stream?subscribe=self"))

    def bridge_client(self, ser_out):
        print("bridge_client started")
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        asyncio.get_event_loop().create_task(SignalKClient.write_data(ser_out))
        asyncio.get_event_loop().run_until_complete(
            SignalKClient.get_signalk_data("ws://10.10.10.1:3000/signalk/v1/stream?subscribe=self"))


if __name__ == '__main__':
    SignalKClient().print_client()