#!/usr/bin/python

"""

    encoder.py - for encoding images in the pxs file format (WIP)
    By Preston Corless

    required libraries: see imports below

    > ./encoder.py [image].[png/jpg/jpeg/gif/bmp/webp]

    (And yes, I am aware that this code is a mess. I will get to that, though it isn't at the top of my priorities.)

"""

from PIL import Image
import numpy as np
import json
import math
import binascii
import sys


channel_bit_depth = 8
def CHANNEL_BIT_MASK(): return int("0b"+("1"*channel_bit_depth)+("0"*(8-channel_bit_depth)), 2)

def base_convert(n, b):
    result = []
    while n > 0:
        result.insert(0, n % b)
        n = n // b
    return result

def rgb2hex(rgb):
    return '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])

def int2longh(num):
    return '{:08x}'.format(num)

def rgb2fifteen(rgb):
    for i in range(0, len(rgb)):
        rgb[i] = rgb[i] & CHANNEL_BIT_MASK()
    return '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])

def rgb2fhex(rgb):
    final = 0
    for i in range(len(rgb)):
        final |= (rgb[i] >> (8 - channel_bit_depth)) << (channel_bit_depth * (2 - i))
    if channel_bit_depth == 5:
        return "{:04x}".format(final)
    else:
        return "{:06x}".format(final)

# Load image and read contents

in_file = sys.argv[1] if len(sys.argv) > 1 else input("File to open: ") # use the kwarg, else ask
img = Image.open(in_file).convert("RGB")

output_type = ""

OUTPUT_OPTIONS = ["J", "B", "J24", "B24"]

while output_type.upper() not in OUTPUT_OPTIONS:
    output_type = input("Target filetype?\n  J: JSON (15-bit color)\n  B: Binary (pxs) (15-bit color)\n  J24: JSON (24-bit color)\n  B24: Binary (24-bit color))\n: ")

image = np.array(img).tolist()

dimensions = (img.size[0], img.size[1])
render_limit = 2 ** math.ceil(math.log(max(img.size[0], img.size[1]), 2))

lim = render_limit ** 2 # number of pixels needed


def calc_coord(idx, width, height):
    b4 = base_convert(idx, 4)
    x = y = 0
    for i in range(len(b4)):
        idx = i + 1
        new_val = b4[len(b4)-idx]
        inc = 2 ** (-idx)
        match new_val:
            case 3:
                x += inc
            case 2:
                y += inc
            case 1:
                x += inc
                y += inc
    return [int(x * width), int(y * height)]

def convert_image(image):
    out = []
    out_hex = int2longh(dimensions[0]) + int2longh(dimensions[1])
    for curr_pix in range(lim):
        [x, y] = calc_coord(curr_pix, render_limit, render_limit)
        if len(image) > y and len(image[y]) > x:
            if channel_bit_depth == 5:
                out.append(rgb2fifteen(image[int(y)][int(x)]))
            else:
                out.append(rgb2hex(image[int(y)][int(x)]))
            hex = rgb2fhex(image[int(y)][int(x)])
            out_hex += hex
    return [out, out_hex]



match output_type:
    case "J":
        channel_bit_depth = 5
        out_json = open(f"out-{output_type}.json", "w")
        json.dump([dimensions, convert_image(image)[0]], out_json)

    case "B":
        channel_bit_depth = 5
        out_bin = open(f"out-{output_type}.pxs", "wb")
        out_bin.write(binascii.unhexlify(convert_image(image)[1]))

    case "J24":
        channel_bit_depth = 8
        out_json = open(f"out-{output_type}.json", "w")
        json.dump([dimensions, convert_image(image)[0]], out_json)

    case "B24":
        channel_bit_depth = 8
        out_bin = open(f"out-{output_type}.pxs", "wb")
        out_bin.write(binascii.unhexlify(convert_image(image)[1]))

