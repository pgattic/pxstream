from PIL import Image
import numpy as np
import json
import math

def base_convert(n, b):
	result = []
	while n > 0:
			result.insert(0, n % b)
			n = n // b
	return result

def rgb2hex(rgb):
    return '#{:02x}{:02x}{:02x}'.format(rgb[0], rgb[1], rgb[2])

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
	for curr_pix in range(lim):
		[x, y] = calc_coord(curr_pix, render_limit, render_limit)
		if len(image) > y and len(image[y]) > x:
			out.append(rgb2hex(image[int(y)][int(x)]))
	return out

encoded_img = [dimensions, convert_image(image)]


out_file = open("out.json", "w")
json.dump(encoded_img, out_file)

