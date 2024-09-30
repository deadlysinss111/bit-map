#include "pch.h"
#include <bitset>

InfectedBitmap::InfectedBitmap() {
	_customInfoHeader = nullptr;
}

InfectedBitmap::~InfectedBitmap() {

}

InfectedBitmap* InfectedBitmap::InfectBmp(BitmapAtHome* src, BYTE* data, int dataLengh) {
	InfectedBitmap* target = InfectedBitmap::InfectBmp(src);
	target->HideData(data, dataLengh);
	return target;
}

InfectedBitmap* InfectedBitmap::InfectBmp(BitmapAtHome* src) {
	InfectedBitmap* infected = new InfectedBitmap();
	infected->_originAddr = new BYTE[src->_size];
	
	infected->_size = src->_size;
	memcpy(infected->_originAddr, src->_originAddr, src->_size);
	infected->_fileHeader = (BITMAPFILEHEADER*)infected->_originAddr;
	infected->_infoHeader = (BITMAPINFOHEADER*)(infected->_originAddr + sizeof(BITMAPFILEHEADER));
	infected->_colorTable = (BYTE*)(infected->_originAddr + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	return infected;
}


//
// HIDE DATA
//

void InfectedBitmap::HideData(BYTE* data, int dataLengh) {

	HideDataHeader(dataLengh);
	//InternalHideData(data, _customInfoHeader->size, CUSTOMHEADERSIZE);
	InternalHideData(data, _customInfoHeader->size, 10);
}

void InfectedBitmap::HideDataHeader(int dataLengh) {
	if (_customInfoHeader != nullptr) {
		delete _customInfoHeader;
	}
	_customInfoHeader = new CustomHeader;
	_customInfoHeader->size = dataLengh;

	int* convertedData = new int[CHEADER_SIZE];
	*convertedData = dataLengh;
	//BYTE* convertedData = (BYTE*)dataLengh;
	InternalHideData((BYTE*)convertedData, 8);

	// just filling with useless value for now
	InternalHideData((BYTE*)convertedData, 2, 8);

	//free(convertedData);
	// TODO hide extension as well
}

void InfectedBitmap::InternalHideData(BYTE* data, int dataLengh, int startingPoint) {
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = _colorTable + 4 * i;
		*targ >>= 2;
		*targ <<= 2;
		BYTE tmp = ( *( (BYTE*)(data + i - startingPoint)) & 0b11000000 ) >> 6;
		*targ |= tmp;

		targ = _colorTable + 4 * i + 1;
		*targ >>= 2;
		*targ <<= 2;
		tmp = ( *((BYTE*)(data + i - startingPoint)) & 0b00110000) >> 4;
		*targ |= tmp;

		targ = _colorTable + 4 * i + 2;
		*targ >>= 2;
		*targ <<= 2;
		tmp = (*((BYTE*)(data + i - startingPoint)) & 0b00001100) >> 2;
		*targ |= tmp;

		targ = _colorTable + 4 * i + 3;
		*targ >>= 2;
		*targ <<= 2;
		BYTE dafuck = *((BYTE*)(data + i - startingPoint));
		tmp = ( dafuck & 0b00000011);
		*targ |= tmp;
		
		/*std::cout << "WRINTING WITNESS; TRY : " << i << "VALUE : " << std::bitset<8>(*(BYTE*)(data + i - startingPoint)) << std::endl;*/
	}
}


//
// READING DATA
//

BYTE* InfectedBitmap::ReadHiddenData() {
	ReadCustomHeader();
	BYTE* data = InternalReadHiddenData(_customInfoHeader->size, 10);
	return data;
	//lets just treat the data as txt for now
}

void InfectedBitmap::ReadCustomHeader() {
	if (_customInfoHeader != nullptr) {
		delete _customInfoHeader;
	}
	_customInfoHeader = new CustomHeader;

	BYTE* data = InternalReadHiddenData(8);
	_customInfoHeader->size = (uint64_t)*data;

	data = InternalReadHiddenData(2, 8);
	_customInfoHeader->extension = (uint16_t)*data;
}

BYTE* InfectedBitmap::InternalReadHiddenData(int dataLengh, int startingPoint) {
	BYTE* data = new BYTE[dataLengh];
	for (int i = startingPoint; i < dataLengh + startingPoint; i++) {
		BYTE* targ = _colorTable + 4 * i;
		BYTE tmp = *targ & 0b00000011;
		data[i - startingPoint] = (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = _colorTable + 4 * i + 1;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = _colorTable + 4 * i + 2;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);
		data[i - startingPoint] <<= 2;

		targ = _colorTable + 4 * i + 3;
		tmp = (*targ & 0b00000011);
		data[i - startingPoint] |= (*targ & 0b00000011);


		//char witness = data[i - startingPoint];
		//std::cout << "witnessing the "<< i << " passage : "<< std::bitset<8>(witness) << std::endl;
	}
	return data;
}