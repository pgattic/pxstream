# Pixel Streaming - An image compression experiment

Developers in the modern web are constantly looking for a means of acceleration, to get content served faster and more efficiently to its users. This has led to the emergence of modern technologies to streamline it. Take, for example, the webp standard, introduced by Google in 2010 and widely supported by web browsers by 2020. 

This is my personal approach to image compression. I do not know where this format will go, or if it will even go anywhere, but in the meantime I personally find it an interesting experiment. 

## About

This format is capable of providing extremely flexible compression of graphics which can also be resolved dynamically as more data comes. It is designed in such a way that more data can be appended to the end of a file in order to up its resolution. Vice versa applies; as more data is removed from the end of the file, the image's resolution decreases. 

This makes it so that the image can be displayed at full size on a webpage while it is being downloaded by the browser, sacrificing no section or chunk of the image by slow data speeds but merely resolution. Thus, a single file from a web server could theoretically be used to serve multiple resolutions of an image (i.e. to mobile users vs. desktop users) simply by cutting off the file past a certain transfer size. 

And the best part? No data is wasted on storing the locations of the pixels; rather, their positions are calculated on-the-fly based on the array index that any given pixel's color information comes in.

Since the inception of this idea, I have not come across any image format that uses such a concept so extensively; though it is in some ways comparable to (but not derived from) libpng's [interlacing and progressive display](http://www.libpng.org/pub/png/book/chapter08.html#png.ch08.div.6) specifications. 

## Progress/Roadmap

| Language | JSON* | Binary (pxs) | Image Header | [By-Pixel Compression](specification.md) |
| ---------------- | :-: | :-: | :-: | :-: |
| Python (Converter) | ✔️ | ✔️ | ✔️ | ❌ |
| HTML5 (Viewer)     | ✔️ | ❌ | ✔️ | ❌ |
| C (Viewer)         | ❌ | ✔️ | ✔️ | ❌ |

\* The JSON format was used for simplicity's sake in testing the algorithm and will soon be removed.

## Usage

Currently the renderer is made with vanilla JavaScript and the encoder in Python for ease of use and comfort of development, but this may change in the future. 

### Web

1. Open the [demopage](https://pgattic.github.io/pxstream) on your web browser. With no image loaded, you will see a demonstration simulating pixel values being consecutively displayed as they would be received. The only difference is that in real applications the loaded pixels are interpolated as it awaits more data. 
2. Insert an array of hex values, in JSON format. (You could use [out128-0321.json](out128-0321.json) from this repo or generate one using the [encoder](encoder.py).)
3. Use the slider below the image to adjust the data cutoff point. 

### Desktop (only tested on Linux, only supports pxs format, not JSON)

For Fedora/RHEL-based:

1. `sudo dnf install freeglut-devel gcc git` for Fedora/RHEL based, `sudo apt update && sudo apt install freeglut3-dev gcc git` for Debian or Ubuntu
2. if you plan on using the encoder, have python3-numpy installed, either through your system package manager or pip.
3. `git clone` this repo and `cd` into it
4. Generate a proper pxs file with `./encode.py [file].[png/jpeg/jpg/...]` and then typing `b24` in the prompt
5. `make`
6. `./pxs [image].pxs`

