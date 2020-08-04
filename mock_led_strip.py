"""
Program to read from serial and emulate rgb led strip.
Author: Joseph Grace
Last Edited: 06/05/2020
"""

from tkinter import Tk, Canvas, ALL, TclError
from serial import Serial
import math
from time import sleep
from colorsys import rgb_to_hls

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600
TOTAL_LEDS = 12
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
    zeros = 0
    while zeros < 3:
        signal = ser.read()
        if signal == b"\0":
            zeros += 1
        else:
            zeros = 0

root = Tk()
root.title("LED Strip Emulator")

canvas = Canvas(root, width=WINDOW_SIZE, height=WINDOW_SIZE, bg="#000")
canvas.pack()

def draw_led(led_number, colour):
    r=int(colour[1:3],16)
    g=int(colour[3:5],16)
    b=int(colour[5:7],16)
    h=rgb_to_hls(r,g,b)[0]*TAU
    center_x = WINDOW_SIZE / 2 + PRIMARY_RADIUS * math.cos(TAU * led_number / TOTAL_LEDS)
    center_y =  WINDOW_SIZE / 2 - PRIMARY_RADIUS * math.sin(TAU * led_number / TOTAL_LEDS)
    p_x = center_x + math.cos(h)*LED_RADIUS
    p_y = center_y - math.sin(h)*LED_RADIUS
    canvas.create_oval(center_x - LED_RADIUS, center_y - LED_RADIUS, center_x + LED_RADIUS, center_y + LED_RADIUS, fill=colour)
    #canvas.create_oval(p_x - LED_RADIUS/3, p_y - LED_RADIUS/3, p_x + LED_RADIUS/3, p_y + LED_RADIUS/3, fill="black")

def draw_all():
    canvas.delete(ALL)
    get_start_signal()
    for i in range(TOTAL_LEDS):
        colour = get_colour()
        draw_led(i, colour)
    canvas.update()

def main(event=None):
    ser.open()
    ser.reset_input_buffer()
    while True:
        try:
            draw_all()
        except TclError:
            break

main()
#root.bind("<space>", main)
#root.mainloop()
