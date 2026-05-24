#!/usr/bin/env python3
import serial
import uinput

PORT = '/dev/serial0'
BAUD = 115200

events = [
    uinput.ABS_X + (-100, 100, 0, 0),
    uinput.ABS_Y + (-100, 100, 0, 0),
    uinput.ABS_RX + (-100, 100, 0, 0),
    uinput.ABS_RY + (-100, 100, 0, 0),

    uinput.BTN_A, uinput.BTN_B, uinput.BTN_X, uinput.BTN_Y,
    uinput.BTN_TL, uinput.BTN_TR, uinput.BTN_SELECT, uinput.BTN_START,
    uinput.BTN_THUMBL, uinput.BTN_THUMBR,
    uinput.BTN_DPAD_UP, uinput.BTN_DPAD_DOWN, uinput.BTN_DPAD_LEFT, uinput.BTN_DPAD_RIGHT,
]

device = uinput.Device(events, name="Arduino Virtual Gamepad")
ser = serial.Serial(PORT, BAUD, timeout=1)

state = {f"SW{i}": 1 for i in range(1, 13)}
state.update({"JS1X": 0, "JS1Y": 0, "JS1SW": 1, "JS2X": 0, "JS2Y": 0, "JS2SW": 1})

button_map = {
    "JS1SW": uinput.BTN_THUMBL,
    "JS2SW": uinput.BTN_THUMBR,
    "SW1": uinput.BTN_A,
    "SW2": uinput.BTN_B,
    "SW3": uinput.BTN_X,
    "SW4": uinput.BTN_Y,
    "SW5": uinput.BTN_TL,
    "SW6": uinput.BTN_TR,
    "SW7": uinput.BTN_SELECT,
    "SW8": uinput.BTN_START,
    "SW9": uinput.BTN_DPAD_UP,
    "SW10": uinput.BTN_DPAD_DOWN,
    "SW11": uinput.BTN_DPAD_LEFT,
    "SW12": uinput.BTN_DPAD_RIGHT,
}

DEADZONE = 8

def clamp(v, lo=-100, hi=100):
    return max(lo, min(hi, v))

def apply_deadzone(v, dz=DEADZONE):
    return 0 if abs(v) < dz else v

def parse_line(line):
    data = {}
    for part in line.split():
        if '=' in part:
            k, v = part.split('=', 1)
            try:
                data[k] = int(v)
            except ValueError:
                pass
    return data

while True:
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if not line:
        continue

    data = parse_line(line)
    if not data:
        continue

    state.update(data)

    device.emit(uinput.ABS_X, apply_deadzone(clamp(state["JS1X"])), syn=False)
    device.emit(uinput.ABS_Y, apply_deadzone(clamp(state["JS1Y"])), syn=False)
    device.emit(uinput.ABS_RX, apply_deadzone(clamp(state["JS2X"])), syn=False)
    device.emit(uinput.ABS_RY, apply_deadzone(clamp(state["JS2Y"])), syn=False)

    for key, code in button_map.items():
        if key in state:
            pressed = 0 if state[key] else 1
            device.emit(code, pressed, syn=False)

    device.syn()
