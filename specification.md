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

## Note

 - The byte 0x`c0` (%`11000000` or relative -0) is the only byte that doesn't have any use. Possible expansion upon this could be helpful. (idea: Could be the separator of frames in an animation)
 - Technically, if a pixel is fully specified with 5 bits, that means it was different enough from its parent to require said specification as opposed to the relative spec. This theoretically could reduce further the need for specificity for the 15-bit method, allowing either smaller data size or higher emulated bit depth.



## Version 1:
 - Headerless, power-of-2 square image, using 15-bit color depth. Nothing too fancy. 


## Potential Resources:
 - https://www.w3schools.com/tags/canvas_putimagedata.asp
 - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array
 - https://developer.mozilla.org/en-US/docs/Web/API/FileReader/readAsArrayBuffer


## Ideas:
 - per-pixel "Finality" bit flag (if set, all future overlapping pixels should be treated as out-of-bounds and skipped) - would be nonexistant in final layer of pixels. It could be used in place of the storage type bitflag, depending on data savings
