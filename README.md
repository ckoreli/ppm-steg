# ppm-steg
A simple steganographic program which hides/ unhides a text file inside of a .ppm image.
It shall hide the bits of the cleartext file in the LSBs of a selectable color channel of the image file and generate a new image file (now containing the cleartext file).

## Usage
After compilation, the program shall be called as follows from the command line:
```sh
steg.exe -i IMAGE_FILE  { -r | -b | -g }  [ -c CLEARTEXT_FILE -o OUTPUT_IMAGE_FILE ]
```
* `-i IMAGE_FILE`         - Specifies the .ppm image file to hide/ unhide text in(required in both modes).
* `{ -r | -b | -g }`      - Specifies the color channel to hide/ unhide data in/ from(required in both modes, only one may be specified).
* `-c CLEARTEXT_FILE`     - Specifies the text file to be hidden in `IMAGE_FILE`.
* `-o OUTPUT_IMAGE_FILE`  - Specifies the output image in which the text will be hidden.
* If only `-i` and color channel are specified, the program will try to unhide data from the image.
* If `-c` and `-o` are specified in addition, the program will try to hide data in the image.

## Hiding
The program takes an image file and cleartext file as input.
Checks if the image is large enough to hide the header as well as the text itself.

### Header
```
STEG\n
<CLEARTEXT_FILENAME>\n
<SIZE_IN_BYTES>\n
```
* `<CLEARTEXT_FILENAME>` is the name of the input text file.
* `<SIZE_IN_BYTES>` is the size of the text file(in bytes).


