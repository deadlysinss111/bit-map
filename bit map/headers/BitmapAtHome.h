#pragma once

class BitmapAtHome {
public:
	BitmapAtHome();
	~BitmapAtHome();

	void LoadFile(const char* addr);
	void SaveAsFile(const char* name);

	void SwapRnB(int colorTableSize);
	bool ChangePixelAt(int x, int y, int rgb[3]);
	void Upscale(int iterations = 1);

	//TODO do accessibility levels properly later
public:
	int _size;
	BYTE* _originAddr;

	BITMAPFILEHEADER* _fileHeader;
	BITMAPINFOHEADER* _infoHeader;
	BYTE* _colorTable;

	void InternalUpscale();
};