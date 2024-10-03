#include "pch.h"

CImage::CImage(BYTE* buffer, int width, int height, int bitsPerPixel) {
	_colorBuffer = buffer;
	_width = width;
	_height = height;
	_bitsPerPixel = bitsPerPixel;
	_bufLengh = width * height ;
}

CImage::~CImage() {
	delete _colorBuffer;
}