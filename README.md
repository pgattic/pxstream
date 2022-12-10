# Pixel Streaming - An image compression experiment

Developers in the modern web are constantly looking for a means of acceleration, to get content served faster and more efficiently to its users. This has led to the emergence of modern technologies to streamline it. Take, for example, the webp standard, introduced by Google in 2010 and widely supported by web browsers by 2020. 

This is my personal approach to image compression. I do not know where this format will go, or if it will even go anywhere, but in the meantime I think it is an experiment that will interest you. 

## About

This format is meant to provide extremely flexible compression of graphics which can also be resolved dynamically as more data comes. It is written in such a way that as more data is appended to the end of the file, the more resolved the image gets. Vice versa applies; as more data is removed from the end of the file, the image's resolution decreases. 

This makes it so that the image can be displayed at full size on a webpage while it is being downloaded by the browser, sacrificing no section or chunk of the image by slow data speeds but merely resolution. Thus, a single file from a web server could theoretically be used to serve multiple resolutions of an image (i.e. to mobile users vs. desktop users) simply by cutting off the file past a certain transfer size. 

And the best part? No data is wasted on storing the locations of the pixels; rather, their positions are calculated on-the-fly based on the array index that any given pixel's RGB data comes in.

Since the inception of this idea, I have not come across any image format that uses such a concept so extensively; though it is in some ways comparable to (but not derived from) libpng's [interlacing and progressive display](http://www.libpng.org/pub/png/book/chapter08.html#png.ch08.div.6) specifications. 

## Progress

This is currently in a proof-of-concept state, with just an HTML-based demo, and an admittedly bloated .json file acting as the image data's only medium of storage. It is only capable of square images, and unless the original image's resolution is a power of 2, it will be lossy. However, this will not always be the case going forward. 

## Usage

Currently the renderer is made with vanilla JavaScript and the encoder in Python for ease of use and comfort of development, but this may change in the future. 

1. Open the [demopage](https://pgattic.github.io/pxstream) on your web browser. With no image loaded, you will see a demonstration simulating pixel values being displayed as they would be received on a verry slow internet connection. The only difference is that in real applications the loaded pixels are interpolated as it awaits more data. 
2. Insert a properly-encoded RGB value array, in JSON format. (You could use [out128-0321.json](out128-0321.json) from this repo or generate one from any python3-PIL supported image using the [encoder](encoder.py).)
3. Use the slider below the image to adjust the data cutoff point. 
