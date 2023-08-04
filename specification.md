# Pixel Stream (PXS) - File Specs (Draft)

## 1. Header

8 bytes - PXS file signature (png signature but with PXS instead of PNG): 0x`89  50  58  53  0d  0a  1a  0a`

8 bytes - image resolution (u32 for width, u32 for height)

## 2. Data

Pixels are stored in one of two methods, to optimize compression. Their storage type is specified using the first bit of the color. 

0: standard 15-bit color (5 bits per channel, 16 bits total)

1: relative to parent -
 - Second bit: sign of relativity (0 for positive, 1 for negative)
 - Next 6 bits: 2 bits per color channel of relative color difference (up to 3)
 - The "parent" pixel's address is calculated from clearing the first set bit present in the address

## Version 1:
 - Headerless, power-of-2 square image, using 15-bit color depth. Nothing too fancy. 


## Potential Resources:
 - https://www.w3schools.com/tags/canvas_putimagedata.asp
 - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array
 - https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsArrayBuffer


## Notes:


 - The byte 0x`c0` (%`11000000` or relative -0) is the only byte that doesn't have any use. Possible expansion upon this could be helpful. (idea: Could be the separator of frames in an animation)
 - Technically, if a pixel is fully specified with 5 bits, that means it was different enough from its parent to require said specification as opposed to the relative spec. This theoretically could reduce further the need for specificity for the 15-bit method, allowing either smaller data size or higher emulated bit depth.
 - Idea - per-pixel "Finality" bit flag (if set, all future overlapping pixels should be treated as out-of-bounds, automatically copy the parent's color, and be skipped) - would be nonexistant in first few an final layer of pixels. It could be used in place of the storage type bitflag, depending on data savings
 - rewrite in a more cuncurrency-friendly language (this encoding method has large potential to take advantage of that)
 - allow header to specify what kinds of channels the image will use (RGBA, HSV, RBY etc.), how many bits are in each channel, or even if the file doesn't declare any color/channel/dimension information (could be useful for things like video streaming or animated images). 
   - There could even be less bits in certain channels (i.e. in images that don't use a lot of red colors), wouldn't that be neat? I know that the L in HSL goes only from 1-100, for example, but there is no reason to not impose restrictive ranges on color channels as well, for the sake of compression.
 - header should specify if the data for the pixels are each byte-aligned and 0-padded to the end of their bytes. This could optimize the image encoding for speed versus size, perhaps?
 - Thanks to the guess-and-check cycle of placing pixels on the image, it could be used to store images in any shape, beyond just rectangles (such as a circle). This would make this file format the only rasterized image format that I am aware of which supports non-rectangukar shapes!

## Thoughts on coordinate pushing order

The current implementation pairs up every two bits of a pixel's index to find out how much to push it left and right, and it pairs them thus:

| Bin | Quar | Left Push | Down Push |
| --- | ---- | --------- | --------- |
| 00  | 0    | +0        | +0        |
| 01  | 1    | +Unit     | +Unit     |
| 10  | 2    | +0        | +Unit     |
| 11  | 3    | +Unit     | +0        |

One benefit of this is that the order of drawing pixels makes the image look much more aesthetically pleasing and complete even when not completely drawn. However, it eliminates certain mathematical shortcuts to calculate the pixel position of a given pixel. 

If it were to be though if this way:

| Bin | Quar | Left Push | Down Push |
| --- | ---- | --------- | --------- |
| 00  | 0    | +0        | +0        |
| 01  | 1    | +0        | +Unit     |
| 10  | 2    | +Unit     | +0        |
| 11  | 3    | +Unit     | +Unit     |

Notice that the leftward and downward pushes line up with the positions of the set bits in the Bin column. This makes it so that we can take an address of any size (using 8 bits as an example), containing alternating leftward and downward bits (0xldldldld), we need only rearrange the bits so they appear as (0xlllldddd), and reverse the bits' order (0xddddllll). Now, the first half of the number, 0xdddd, is the y coordinate and the second half, 0xllll, is the x position. 

This results in a *much* more efficient position calculation; however (compare to the code found in decoder.c's `calcPos` function, which uses iteration and switches), the image looks worse when partially remdered. Keep in mind, however, that this alternative is in no means a sacrifice of encoding efficiency, rather one of appearance to the human eye. 

Not all hope is lost, though: a few bitwise operations can convert from the first table's bits to those of the other. What remains to be determined is an efficient way to isolate the leftward and downward bits without iteration, if possible.