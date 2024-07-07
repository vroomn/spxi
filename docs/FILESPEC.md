# Documentation for the Small Pixel format (.spxi)
#### Effective version of documentation: v0.1

The birth and reason for this image format is to store pixel-art based work in an accesible file format for toying around with. It's intent is to be simple and small, a bitmap that's slightly more compression oriented, and more importantly disconnected from Microsoft. (Also it's a pet project)  
Backwards compatability will be preserved to the extent it doesn't impact the size of the library or hinder progression of standards.

The file is broken into three primary components: the header, Color ID Definition, and the ordered pixel index. The header is the same as other files, containing important metadata about the file such as file size, BPP, version, etc. The Color ID Definitions comes from the notion pixel art has a limited color pallate, and therefore a limited number of colors, meaning there is gains to be had in terms of memory to assign each color to an index. The pixel index is an arbitary length series of indexes that when pieced together will form the stored image. The pixel index can use of run length encoding if the file creator deemed it worthwile (flag within the header)

### A Small Note for Endianness
Data is stored in little-endian, due to the fact it's rather universal.

## Header
### Magic Number
__Bytes: 4__  
The magic number sequence is "spxi" in ascii, or 0x73 0x70 0x78 0x69

### Version
__Bytes: 1__  
The version of the file will be provided as one of these (only one at time of writing):
1. 0x0 -> Version 0.1 [Current]

### File Size
__Bytes: 4__  
The size of the file, header included

### Width
__Bytes: 4__  
The width of the image in pixels

### Height
__Bytes: 4__  
The height of the image in pixels

### Bits Per Pixel
__Bytes: 1__  
The Bits Per Pixel, or BPP, is an indication of the color fidelity of the image, supporting:
1. 8 -> 8 BPP; Intended for greyscale or black-white images, scaling from 0-255, where 0 is pure black and 255 is pure white. No opacity is present.  
To convert simply set each color channel of 0-255 RGB to the 8BPP value for consistent greyscale color
2. 24 -> 24 BPP; Color rendering but lacks opacity, each color is 0-255.
3. 32 -> 32 BPP; The same as 24 BPP but has the capacity for opacity

### Color Bitmasks
__Bytes: 4__  
24 and 32 BPP color bitmasks, the size of the bitmap is locked to 4 bytes to standardize header size. As the file is read little-endian, 24 BPP can ignore the alpha byte. 8 BPP completely ignores this section of data, should be 0.

### Flags
__Bytes: 1__  
Each bit of this section of memory is a different configuration flag, as follows:
1. Run Length Encoding -> 1 if encoded, 0 if not. Means the ordered pixel index makes use of RLE store data.
2. 8 or 16 bits are allocated for the color ID's, giving more possible colors but taking up more storage. 0 corresponds to 8 bits (1 byte) and 1 corresponds to 16 bits (2 Bytes).

All bits beyond this should be zero and ignored.

## Color ID definition
The color ID is comprised of two components: the ID number and the corresponding color. The number of possible ID's is set in the flags section of the header, 256 with 8 bit or 65,536 with 16 bit, 16 is a highly unlikely flag due to the intent of the format.

### Number of pairs
__Bytes: 1-2__
This is also controlled by the color ID flag, indicates the number of pairs present. 
__To fully calculate how many bytes there are this formula should suffice:__  
*bytes = Number of pairs * (number of bytes allocated to ID + BPP)*

### Pairs
Read this section based on the number of bytes allocated to an ID + BPP in accordance to the number of pairs, and all should be well.

## Ordered Pixel Index
The ordered pixel index is a series of indexes, with possibly using RLE to further reduce file size. The number of bytes is dependent on the ID flag.

### Number of bytes
__Bytes: 4__
A count of how many bytes are present, can also be interpreted as the size of purely the image, not including header and color ID