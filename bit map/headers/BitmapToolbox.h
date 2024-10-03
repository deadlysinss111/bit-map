#pragma once


class BitmapToolbox{
public:
	BitmapToolbox();
	~BitmapToolbox();

	void SwapRnB(BitmapFile* target,  int colorTableSize);
	bool ChangePixelAt(BitmapFile* target, int x, int y, int rgb[3]);
	void Upscale(BitmapFile* target, int iterations = 1);

	void HideData(BitmapFile* target, BYTE* data, int dataLengh, const char* extension = "txt");
	BYTE* ReadHiddenData(BitmapFile* target, CustomHeader* headerAddr = nullptr);

	
private:
	void InternalUpscale(BitmapFile* target);

	void HideDataHeader(BitmapFile* target, int dataLengh, const char* extension);
	void InternalHideData(BitmapFile* target, BYTE* data, int dataLengh, int startingPoint = 0);

	CustomHeader ReadCustomHeader(BitmapFile* target);
	BYTE* InternalReadHiddenData(BitmapFile* target, int dataLengh, int startingPoint = 0);
};