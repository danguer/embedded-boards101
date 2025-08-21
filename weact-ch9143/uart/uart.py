import serial
import threading
import argparse
import time

keep_reading = True
send_idx = 0

def read_serial(ser: serial.Serial):
    while keep_reading:
        if ser.in_waiting > 0:
            waiting = ser.in_waiting
            line = ser.read(waiting).decode("utf-8")
            print("Serial Input: ", line)

def send_at_command(ser: serial.Serial, command: str) -> str:
    # send command
    ser.write(f"{command}\r\n".encode("ascii"))
    # read command back
    return ser.readline().decode("ascii").strip()

parser = argparse.ArgumentParser(prog="CH9143 Uart Example")
parser.add_argument(
    "--use-at-commands", 
    help="Use AT Commands", 
    action=argparse.BooleanOptionalAction,
)
parser.add_argument(
    "serial_port", 
    help="Serial port like /dev/ttyACM0",
)
args = parser.parse_args()

params = {
    "port": args.serial_port,
    "baudrate": 115200,
    "bytesize": serial.EIGHTBITS, 
    "parity": serial.PARITY_NONE, 
    "stopbits": serial.STOPBITS_ONE, 
    "timeout": 1,
}

print("Opening port: ", args.serial_port)
with serial.Serial(**params) as ser:
    if args.use_at_commands:
        print("Sending AT... (checking if AT is enabled)")
        print("Response: ", send_at_command(ser, "AT..."))
        print("Sending AT+MAC? (checking current MAC)")
        mac = send_at_command(ser, "AT+MAC?")
        if mac:
            mac = mac.split(":")
            mac.reverse()
            mac = ":".join(mac)

        print("Response: ", mac)
        print("Sending AT+CCADD? (checking connected MAC)")
        print("Response: ", send_at_command(ser, "AT+CCADD?"))
         

    print("Creating background receiving thread")
    thread_read = threading.Thread(target=read_serial, args=(ser,))
    thread_read.start()

    try:
        while True:
            print(f"Sending ping {send_idx}")
            ser.write(f"UART PING {send_idx}".encode("ascii"))
            send_idx += 1
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        print("Finishing reading thread")
        keep_reading = False
        thread_read.join()
