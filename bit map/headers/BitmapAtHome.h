#pragma once

class BitmapAtHome {
public:
	BitmapAtHome();
	~BitmapAtHome();

	void LoadFile(const char* addr);
	void WriteInFile(const char* name);

	void SwapRnB(int colorTableSize);
	bool ChangePixelAt(int x, int y, int rgb[3]);

private:
	int _size;
	BYTE* _originAddr;

	BITMAPFILEHEADER* _fileHeader;
	BITMAPINFOHEADER* _infoHeader;
	BYTE* _colorTable;
};