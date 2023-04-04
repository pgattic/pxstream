from PIL import Image
import numpy as np
import json
import math
import binascii


CHANNEL_BIT_DEPTH = 8
CHANNEL_BIT_MASK = int("0b"+("1"*CHANNEL_BIT_DEPTH)+("0"*(8-CHANNEL_BIT_DEPTH)), 2)

def base_convert(n, b):
	result = []
	while n > 0:
		result.insert(0, n % b)
		n = n // b
	return result

def rgb2hex(rgb):
	return '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])


def rgb2fifteen(rgb):
	for i in range(0, len(rgb)):
		rgb[i] = rgb[i] & CHANNEL_BIT_MASK
	return '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])

def rgb2fhex(rgb):
	final = 0
	for i in range(len(rgb)):
		final |= (rgb[i] >> (8 - CHANNEL_BIT_DEPTH)) << (CHANNEL_BIT_DEPTH * (2 - i))
	if CHANNEL_BIT_DEPTH == 5:
		return "{:04x}".format(final)
	else:
		return "{:06x}".format(final)

# Load image and read contents

in_file = input("File to open: ")

img = Image.open(in_file).convert("RGB")

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
	out_hex = ""
	for curr_pix in range(lim):
		[x, y] = calc_coord(curr_pix, render_limit, render_limit)
		if len(image) > y and len(image[y]) > x:
			out.append(rgb2fifteen(image[int(y)][int(x)]))
			hex = rgb2fhex(image[int(y)][int(x)])
			out_hex += hex
#	return out
	return out_hex


out_bin = open("out-8bit.pxs", "wb")
out_bin.write(binascii.unhexlify(convert_image(image)))

#out_json = open("out.json", "w")
#json.dump([dimensions, convert_image(image)], out_json)

