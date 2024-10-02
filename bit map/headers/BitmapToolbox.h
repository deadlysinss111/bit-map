#pragma once

#define CUSTOMHEADERSIZE 10
#define CHEADER_SIZE 8
#define CHEADER_EXTENSION 2

struct CustomHeader {
	uint64_t size;
	uint16_t extension;
};

class BitmapToolbox{
public:
	BitmapToolbox();
	~BitmapToolbox();

	void SwapRnB(BitmapFile* target,  int colorTableSize);
	bool ChangePixelAt(BitmapFile* target, int x, int y, int rgb[3]);
	void Upscale(BitmapFile* target, int iterations = 1);

	void HideData(BitmapFile* target, BYTE* data, int dataLengh);
	BYTE* ReadHiddenData(BitmapFile* target);

	
private:
	void InternalUpscale(BitmapFile* target);

	void HideDataHeader(BitmapFile* target, int dataLengh);
	void InternalHideData(BitmapFile* target, BYTE* data, int dataLengh, int startingPoint = 0);

	CustomHeader ReadCustomHeader(BitmapFile* target);
	BYTE* InternalReadHiddenData(BitmapFile* target, int dataLengh, int startingPoint = 0);
};