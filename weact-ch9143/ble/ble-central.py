import asyncio
import argparse
import time
import sys
from bleak import *

async def discover_mac():
    scanner = BleakScanner()
    device = await scanner.find_device_by_name("CH9143BLE2U")
    if not device:
        print("Failed to find CH9143BLE2U device")
        sys.exit(1)

    print("Found device:", device)
    return device.address

def notify_callback(sender: BleakGATTCharacteristic, data: bytearray):
    print("Got from device:", data.decode("utf-8"))

async def connect_to_mac(device_mac: str):
    print("Connecting to mac", device_mac)
    client = BleakClient(device_mac)
    await client.connect()
    print("Listing services")
    uuid_write = None
    uuid_read = None
    for service in client.services:
        print("[Service] {0}: {1}".format(service.uuid, service.description))
        if service.uuid[0:4] == "0000" and service.uuid[4:8] == "fff0":
            print("Found transparent service") 
            for char in service.characteristics:
                # print("Char", char, char.properties) # debug printing
                if char.uuid[4:8] == "fff1":
                    uuid_read = char.uuid
                elif char.uuid[4:8] == "fff2":
                    uuid_write = char.uuid

    if uuid_read and uuid_write:
        print(
            "Found needed characteristics, read:", 
            uuid_read, 
            ", write:",
            uuid_write,
        )
        send_idx = 0
        await client.start_notify(uuid_read, notify_callback)
        try:
            while True:
                print(f"Sending ping {send_idx}")
                await client.write_gatt_char(
                    uuid_write,
                    f"HOST PING {send_idx}".encode("ascii"),
                )
                send_idx += 1
                time.sleep(1)
        except KeyboardInterrupt:
            pass
        finally:
            print("Finishing app")

async def main():
    parser = argparse.ArgumentParser(prog="BLE CH9143 Host")
    parser.add_argument(
        "--device-mac",
        default=None,
        type=str,
        help="Device MAC to connect",
    )

    args = parser.parse_args()
    if not args.device_mac:
        device_mac = await discover_mac()
    else:
        device_mac = args.device_mac

    await connect_to_mac(device_mac)

asyncio.run(main())
