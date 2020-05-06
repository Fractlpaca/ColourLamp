"""
Program to read from serial and emulate rgb led strip.
Author: Joseph Grace
Last Edited: 06/05/2020
"""

from tkinter import Tk, Canvas, ALL, TclError
from serial import Serial
import math
from time import sleep

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 57600
TOTAL_LEDS = 16
PRIMARY_RADIUS = 250
LED_RADIUS = 40
WINDOW_SIZE = 600
TAU = 2 * math.pi

ser = Serial()
ser.port = SERIAL_PORT
ser.baudrate = BAUD_RATE

def get_colour():
    return "#" + ser.read(3).hex()

def get_start_signal():
    signal = b""
    while signal != b"\0":
        signal = ser.read()

root = Tk()
root.title("LED Strip Emulator")

canvas = Canvas(root, width=WINDOW_SIZE, height=WINDOW_SIZE, bg="#000")
canvas.pack()

def draw_led(led_number, colour):
    center_x = WINDOW_SIZE / 2 + PRIMARY_RADIUS * math.cos(TAU * led_number / TOTAL_LEDS)
    center_y =  WINDOW_SIZE / 2 - PRIMARY_RADIUS * math.sin(TAU * led_number / TOTAL_LEDS)
    canvas.create_oval(center_x - LED_RADIUS, center_y - LED_RADIUS, center_x + LED_RADIUS, center_y + LED_RADIUS, fill=colour)

def draw_all():
    canvas.delete(ALL)
    #get_start_signal()
    for i in range(TOTAL_LEDS):
        colour = "#ff0" #get_colour()
        draw_led(i, colour)
    canvas.update()

def main(event=None):
    while True:
        sleep(0.1)
        try:
            draw_all()
        except TclError:
            break

root.bind("<space>", main)
root.mainloop()
