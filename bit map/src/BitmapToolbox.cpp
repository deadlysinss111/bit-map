#include "pch.h"
#include <bitset>

BitmapToolbox::BitmapToolbox() {

}

BitmapToolbox::~BitmapToolbox() {

}

void BitmapToolbox::SwapRnB(BitmapFile* target, int colorTableSize) {
	for (int i = 0; i < colorTableSize; i += 3) {
		BYTE buf = *(target->_pixelData + i);
		*(target->_pixelData + i) = *(target->_pixelData + i + 2);
		*(target->_pixelData + i + 2) = buf;
	}
}

bool BitmapToolbox::ChangePixelAt(BitmapFile* target, int x, int y, int rgb[3]) {
	int width = target->_infoHeader->biWidth;
	int height = target->_infoHeader->biHeight;

	if (x < 0 || x >= width || y < 0 || y >= height) return false;

	target->_pixelData[3 * (x + width * (height - y))] = rgb[0];
	target->_pixelData[3 * (x + width * (height - y)) + 1] = rgb[1];
	target->_pixelData[3 * (x + width * (height - y)) + 2] = rgb[2];
	return true;
}

void BitmapToolbox::Upscale(BitmapFile* target, int iterations) {
	for (int i = 0; i < iterations; i++) {
		InternalUpscale(target);
	}
}

// Double the size of the bmp
void BitmapToolbox::InternalUpscale(BitmapFile* target) {
	int newWidth = target->_infoHeader->biWidth * 2;
	int newHeight = target->_infoHeader->biHeight * 2;
	int bytesPerPixel = target->_infoHeader->biBitCount / 8;

	int newRowSize = ((newWidth * target->_infoHeader->biBitCount + 31) / 32) * 4;

	BYTE* addr = new BYTE[newRowSize * newHeight + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];

	memcpy(addr, target->_buffer, sizeof(BITMAPFILEHEADER));
	memcpy(target->_fileHeader, addr, sizeof(BITMAPFILEHEADER));
	memcpy(addr + sizeof(BITMAPFILEHEADER), target->_buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	memcpy(target->_infoHeader, target->_buffer + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	BYTE* colorAddr = addr + target->_fileHeader->bfOffBits;

	for (int j = 0; j < target->_infoHeader->biHeight; j++) {
		for (int i = 0; i < target->_infoHeader->biWidth; i++) {

			// Let's retrieve the pixel we want to copy
			int srcIndex = (i + j * target->_infoHeader->biWidth) * bytesPerPixel;
			BYTE* pixelData = target->_pixelData + srcIndex;

			// We are looking for the base destination in our copy bmp
			int destIndex = (i*2 + j*2* newWidth) * bytesPerPixel;

			// For each of the r, g and b value
			for (int b = 0; b < bytesPerPixel; b++) {
				// We copy it into a square of 4 pixels (the first one being the base destination we searched for just before) into our copy bmp
				colorAddr[destIndex + b] = pixelData[b];
				colorAddr[destIndex + b + bytesPerPixel] = pixelData[b];
				colorAddr[destIndex + b + newWidth * bytesPerPixel] = pixelData[b];
				colorAddr[destIndex + b + newWidth * bytesPerPixel + bytesPerPixel] = pixelData[b];
			}
		}
	}

	target->_infoHeader->biWidth = newWidth;
	target->_infoHeader->biHeight = newHeight;
	target->_infoHeader->biSizeImage = newRowSize * newHeight;
	target->_size = newRowSize * newHeight + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	delete target->_buffer;
	target->_buffer = addr;
	target->_pixelData = colorAddr;
	target->_fileHeader->bfSize = newRowSize * newHeight + sizeof(BITMAPINFOHEADER);
}


//
// HIDE DATA
//

void BitmapToolbox::HideData(BitmapFile* target, BYTE* data, int dataLengh, const char* extension) {

	HideDataHeader(target, dataLengh, extension);
	InternalHideData(target, data, dataLengh, CUSTOMHEADERSIZE);
}

void BitmapToolbox::HideDataHeader(BitmapFile* target, int dataLengh, const char* extension) {

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

void BitmapToolbox::InternalHideData(BitmapFile* target, BYTE* data, int dataLengh, int startingPoint) {
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = target->_pixelData + 4 * i;
		*targ >>= 2;
		*targ <<= 2;
		BYTE tmp = (data[i - startingPoint] & 0b11000000) >> 6;
		*targ |= tmp;

		targ = target->_pixelData + 4 * i + 1;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (data[i - startingPoint] & 0b00110000) >> 4;
		*targ |= tmp;

		targ = target->_pixelData + 4 * i + 2;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (data[i - startingPoint] & 0b00001100) >> 2;
		*targ |= tmp;

		targ = target->_pixelData + 4 * i + 3;
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

BYTE* BitmapToolbox::ReadHiddenData(BitmapFile* target, CustomHeader* headerAddr) {
	CustomHeader header = ReadCustomHeader(target);
	if (headerAddr != nullptr) {
		*headerAddr = header;
	}
	BYTE* data = InternalReadHiddenData(target, header.size, CUSTOMHEADERSIZE);
	return data;
	//lets just treat the data as txt for now
}

CustomHeader BitmapToolbox::ReadCustomHeader(BitmapFile* target) {

	CustomHeader resultHeader;

	BYTE* data = InternalReadHiddenData(target, CHEADER_SIZE);
	resultHeader.size = *((int*)data);


	data = InternalReadHiddenData(target, CHEADER_EXTENSION, CHEADER_SIZE);
	memcpy(&(resultHeader.extension), &data, CHEADER_EXTENSION);


	return resultHeader;
}

BYTE* BitmapToolbox::InternalReadHiddenData(BitmapFile* target, int dataLengh, int startingPoint) {
	BYTE* data = new BYTE[dataLengh];
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = target->_pixelData + 4 * i;
		BYTE tmp = *targ & 0b00000011;
		data[i - startingPoint] = (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_pixelData + 4 * i + 1;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_pixelData + 4 * i + 2;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = target->_pixelData + 4 * i + 3;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);

	}
	return data;
}