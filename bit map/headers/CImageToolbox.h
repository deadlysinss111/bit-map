#pragma once

#define CUSTOMHEADERSIZE 16
#define CHEADER_SIZE 4
#define CHEADER_EXTENSION 8

struct CustomHeader {
	uint32_t size;
	BYTE* extension;
};

class CImageToolbox
{
public:
	CImageToolbox();
	~CImageToolbox();

public:
	// convert functions
	CImage* BitmapToCImage(RawFile*);
	CImage* BitmapToCImage(BitmapFile*);

	RawFile* CImageToBitmap(CImage*);


public:
	// Tools

	bool ChangePixelAt(CImage* target, int x, int y, int rgb[3]);
	void Upscale(CImage* target, int iterations = 1);

	void HideData(CImage* target, BYTE* data, int dataLengh, const char* extension = "txt");
	BYTE* ReadHiddenData(CImage* target, CustomHeader* headerAddr = nullptr);


private:
	void InternalUpscale(CImage* target);

	void HideDataHeader(CImage* target, int dataLengh, const char* extension);
	void InternalHideData(CImage* target, BYTE* data, int dataLengh, int startingPoint = 0);

	CustomHeader ReadCustomHeader(CImage* target);
	BYTE* InternalReadHiddenData(CImage* target, int dataLengh, int startingPoint = 0);

	void SwapRnB(BYTE* target, int colorTableSize);
};

