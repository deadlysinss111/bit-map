#include "pch.h"

CImageToolbox::CImageToolbox() {

}

CImageToolbox::~CImageToolbox() {

}

void CImageToolbox::SwapRnB(BYTE* target, int colorTableSize) {
	for (int i = 0; i < colorTableSize; i += 3) {
		BYTE buf = *(target + i);
		*(target + i) = *(target + i + 2);
		*(target + i + 2) = buf;
	}
}

bool CImageToolbox::ChangePixelAt(CImage* target, int x, int y, int rgb[3]) {
	int width = target->_width;
	int height = target->_height;

	if (x < 0 || x >= width || y < 0 || y >= height) return false;

	target->_colorBuffer[3 * (x + width * (height - y))] = rgb[0];
	target->_colorBuffer[3 * (x + width * (height - y)) + 1] = rgb[1];
	target->_colorBuffer[3 * (x + width * (height - y)) + 2] = rgb[2];
	return true;
}

void CImageToolbox::Upscale(CImage* target, int iterations) {
	for (int i = 0; i < iterations; i++) {
		InternalUpscale(target);
	}
}

// Double the size of the bmp
void CImageToolbox::InternalUpscale(CImage* target) {
	int newWidth = target->_width * 2;
	int newHeight = target->_height * 2;
	int bytesPerPixel = target->_bitsPerPixel / 8;

	int newRowSize = ((newWidth * target->_bitsPerPixel+ 31) / 32) * 4;

	BYTE* addr = new BYTE[newRowSize * newHeight];

	for (int j = 0; j < target->_height; j++) {
		for (int i = 0; i < target->_width; i++) {

			// Let's retrieve the pixel we want to copy
			int srcIndex = (i + j * target->_width) * bytesPerPixel;
			BYTE* pixelData = target->_colorBuffer + srcIndex;

			// We are looking for the base destination in our copy bmp
			int destIndex = (i * 2 + j * 2 * newWidth) * bytesPerPixel;

			// For each of the r, g and b value
			for (int b = 0; b < bytesPerPixel; b++) {
				// We copy it into a square of 4 pixels (the first one being the base destination we searched for just before) into our copy bmp
				addr[destIndex + b] = pixelData[b];
				addr[destIndex + b + bytesPerPixel] = pixelData[b];
				addr[destIndex + b + newWidth * bytesPerPixel] = pixelData[b];
				addr[destIndex + b + newWidth * bytesPerPixel + bytesPerPixel] = pixelData[b];
			}
		}
	}

	target->_width = newWidth;
	target->_height = newHeight;
	target->_bufLengh = newRowSize * newHeight;
	delete target->_colorBuffer;
	target->_colorBuffer = addr;
}


//
// CONVERTIONS
//

CImage* CImageToolbox::BitmapToCImage(RawFile* file) {

	BITMAPINFOHEADER infoHeader;
	memcpy(&infoHeader, file->_buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

	BYTE* buf = new BYTE[infoHeader.biSize];
	memcpy(buf, file->_buffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), infoHeader.biSizeImage);

	SwapRnB(buf, infoHeader.biSize);

	CImage* image = new CImage(buf, infoHeader.biWidth, infoHeader.biHeight, infoHeader.biBitCount);

	return image;
}

CImage* CImageToolbox::BitmapToCImage(BitmapFile* file) {
	return BitmapToCImage((RawFile*)file);
}

RawFile* CImageToolbox::CImageToBitmap(CImage* img) {
	BITMAPFILEHEADER fileHeader;

	fileHeader.bfType = 0x4d42;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + img->_bitsPerPixel * img->_bufLengh;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER infoHeader;

	infoHeader.biSize = img->_bitsPerPixel * img->_bufLengh;
	infoHeader.biWidth = img->_width;
	infoHeader.biHeight = img->_height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = img->_bitsPerPixel;
	infoHeader.biCompression = BI_RGB;
	// may be a problem from there
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	BYTE* buffer = new BYTE[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (img->_bitsPerPixel/8) * img->_bufLengh];
	memcpy(buffer, &fileHeader, sizeof(BITMAPFILEHEADER));
	memcpy(buffer + sizeof(BITMAPFILEHEADER), &infoHeader, sizeof(BITMAPINFOHEADER));
	memcpy(buffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), img->_colorBuffer, (img->_bitsPerPixel / 8) * img->_bufLengh);

	RawFile* file = new RawFile();
	file->_buffer = buffer;
	file->_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (img->_bitsPerPixel / 8) * img->_bufLengh;

	return file;
}



//
// HIDE DATA
//

void CImageToolbox::HideData(CImage* target, BYTE* data, int dataLengh, const char* extension) {

	HideDataHeader(target, dataLengh, extension);
	InternalHideData(target, data, dataLengh, CUSTOMHEADERSIZE);
}

void CImageToolbox::HideDataHeader(CImage* target, int dataLengh, const char* extension) {

	BYTE* convertedData = new BYTE[CHEADER_SIZE];
	memcpy(convertedData, &dataLengh, CHEADER_SIZE);
	//BYTE* convertedData = (BYTE*)dataLengh;
	InternalHideData(target, convertedData, CHEADER_SIZE);


	delete convertedData;

	BYTE* convertedExtension = new BYTE[CHEADER_EXTENSION];
	memcpy(convertedExtension, extension, CHEADER_EXTENSION);
	InternalHideData(target, convertedExtension, CHEADER_EXTENSION, CHEADER_SIZE);

	delete convertedExtension;
}

void CImageToolbox::InternalHideData(CImage* target, BYTE* data, int dataLengh, int startingPoint) {
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = target->_colorBuffer + 4 * i;
		*targ >>= 2;
		*targ <<= 2;
		BYTE tmp = (data[i - startingPoint] & 0b11000000) >> 6;
		*targ |= tmp;

		targ = target->_colorBuffer + 4 * i + 1;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (data[i - startingPoint] & 0b00110000) >> 4;
		*targ |= tmp;

		targ = target->_colorBuffer + 4 * i + 2;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (data[i - startingPoint] & 0b00001100) >> 2;
		*targ |= tmp;

		targ = target->_colorBuffer + 4 * i + 3;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (data[i - startingPoint] & 0b00000011);
		*targ |= tmp;

		//std::cout << "WRINTING WITNESS; TRY : " << i << "VALUE : " << std::bitset<8>(data[i - startingPoint]) << " || " << std::bitset<8>(*targ) << std::endl;
	}
}


//
// READING DATA
//

BYTE* CImageToolbox::ReadHiddenData(CImage* target, CustomHeader* headerAddr) {
	CustomHeader header = ReadCustomHeader(target);
	if (headerAddr != nullptr) {
		*headerAddr = header;
	}
	BYTE* data = InternalReadHiddenData(target, header.size, CUSTOMHEADERSIZE);
	return data;
	//lets just treat the data as txt for now
}

CustomHeader CImageToolbox::ReadCustomHeader(CImage* target) {

	CustomHeader resultHeader;

	BYTE* data = InternalReadHiddenData(target, CHEADER_SIZE);
	resultHeader.size = *((int*)data);


	data = InternalReadHiddenData(target, CHEADER_EXTENSION, CHEADER_SIZE);
	memcpy(&(resultHeader.extension), &data, CHEADER_EXTENSION);


	return resultHeader;
}

BYTE* CImageToolbox::InternalReadHiddenData(CImage* target, int dataLengh, int startingPoint) {
	BYTE* data = new BYTE[dataLengh];
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = target->_colorBuffer + 4 * i;
		BYTE tmp = *targ & 0b00000011;
		data[i - startingPoint] = (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_colorBuffer + 4 * i + 1;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_colorBuffer + 4 * i + 2;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_colorBuffer + 4 * i + 3;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);

	}
	return data;
}