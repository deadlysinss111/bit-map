#pragma once
class CImage
{
public:
	CImage(BYTE* buffer, int width, int height, int bitsPerPixel);
	~CImage();

	BYTE* _colorBuffer;
	int _bufLengh;

	int _width;
	int _height;

	int _bitsPerPixel;
};

