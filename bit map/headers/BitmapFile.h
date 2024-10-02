#pragma once

class BitmapFile : public RawFile {
public:
	BitmapFile();
	~BitmapFile();

	void LoadFile(const char* addr);

public:
	BITMAPFILEHEADER* _fileHeader;
	BITMAPINFOHEADER* _infoHeader;
	BYTE* _pixelData;
};