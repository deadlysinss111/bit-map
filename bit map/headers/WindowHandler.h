#pragma once

#define ENCODE_TAB_INDEX 0
#define DECODE_TAB_INDEX 1

#define STATIC_TEXT_STYLE  /* ==> */ WS_VISIBLE | WS_CHILD
#define STATIC_FRAME_STYLE /* ==> */ WS_VISIBLE | WS_CHILD | WS_BORDER
#define BUTTON_STYLE       /* ==> */ WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHBUTTON
#define EDIT_STYLE         /* ==> */ WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN
#define FIXED_EDIT_STYLE   /* ==> */ WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY

#define IDB_HOST 0 
#define IDB_PARASITE 1
#define IDE_PARASITE 2
#define IDB_INJECT_FILE 3
#define IDB_INJECT_TEXT 4
#define IDB_INFECTED 5

#define BMP_PREVIEW_DIMS 200

#define ENCODE_SCREEN_HWND_COUNT 8
#define DECODE_SCREEN_HWND_COUNT 7

struct WindowCustomParam {
	WindowHandler* windowHandler;
	BitmapAtHome* bitmapAtHome;
	HBITMAP* hBmp;
};

class WindowHandler
{
public:
	WindowHandler(HINSTANCE hInstance);
	~WindowHandler();

	void RunWindow();

	HBITMAP CreateBmpHandler(BitmapAtHome* bmp);
	void SetCustomParam(WindowCustomParam* param);
	void CallRedraw();

	// Public since we need main.cpp to fill some of its fields
	static WindowCustomParam _cParam;

private:
	HINSTANCE _hInstance;
	HWND _hWnd;
	HDC _hDc;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void RegisterWindowClass();
	HWND CreateMainWindow();

	/*
	   UI Elements : Tabs
	*/
	HWND _hTabControl;							// Handle to the Tab Control
	wchar_t _encodeTabName[50] = L"Encode";		// duh
	wchar_t _decodeTabName[50] = L"Decode";		// duh

	// Called if the window gets resized to adapt the tabs as well
	HRESULT ResizeTabControl(LPARAM ARGlParam);

	/*
	   UI Elements : Encoding screen
	*/
	static HWND** _hENCODEarrElements;			// Contains all of the pointers to the UI Elements of the Encoding screen
	HWND _hENCODEbtnOpenHostBMP;				// duh
	HWND _hENCODEbtnOpenParasiteFile;			// duh
	HWND _hENCODEbtnInjectFile;					// duh
	HWND _hENCODEbtnInjectMessage;				// duh
	HWND _hENCODEeditParasiteMessage;			// duh
	HWND _hENCODEstaticHostPreview;				// duh
	HWND _hENCODEstaticResultPreview;			// duh
	HWND _hENCODEstaticResultTitle;				// duh

	/*
	   UI Elements : Decoding screen
	*/
	static HWND** _hDECODEarrElements;			// Contains all of the pointers to the UI Elements of the Deconding screen
	HWND _hDECODEbtnOpenInfectedBMP;			// duh
	HWND _hDECODEbtnExtractData;				// duh
	HWND _hDECODEstaticInfectedPreview;			// duh
	HWND _hDECODEstaticMessageTitle;			// duh
	HWND _hDECODEstaticOperationResult;			// duh
	HWND _hDECODEeditOperationResult;			// duh
	HWND _hDECODEeditExtractedData;				// duh

	/*
	   UI Elements : Padding constants
	*/
	short _smolPad = 1;		// Barely noticeable padding UwU
	short _globalPad = 5;	// "Default", got-to padding
	short _strongPad = 13;	// More pronounced padding

	// Creates every UI elements (should be called on construction)
	void CreateUIElements();
};