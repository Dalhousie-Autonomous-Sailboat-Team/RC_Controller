# serial_forwarder.py
# 
# Description:
#   Bidirectional serial port forwarder between two serial ports.
#   Press Ctrl+C to exit gracefully.
#
# Dependencies:
#   pip install pyserial
#
# Usage:
#   python serial_forwarder.py

import serial
import threading
import signal
import sys
import time

# --- Configurable Serial Ports ---
PORT1 = 'COM5'
PORT2 = 'COM11'
BAUDRATE = 9600

# Event to signal threads to stop
stop_event = threading.Event()

def forward(src, dst):
    while not stop_event.is_set():
        if src.in_waiting:
            data = src.read(src.in_waiting)
            dst.write(data)

def cleanup():
    stop_event.set()
    try:
        port1.close()
    except:
        pass
    try:
        port2.close()
    except:
        pass

def signal_handler(sig, frame):
    print("\nExiting...")
    cleanup()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# Try opening serial ports, exit on failure
try:
    port1 = serial.Serial(PORT1, BAUDRATE)
    port2 = serial.Serial(PORT2, BAUDRATE)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    sys.exit(1)

# Start bidirectional forwarding threads
threading.Thread(target=forward, args=(port1, port2), daemon=True).start()
threading.Thread(target=forward, args=(port2, port1), daemon=True).start()

# Keep main thread alive until interrupted
try:
    while not stop_event.is_set():
        time.sleep(0.1)
except KeyboardInterrupt:
    signal_handler(None, None)
