#pragma once

class WindowHandler
{
public:
	WindowHandler(HINSTANCE hInstance);
	~WindowHandler();

	void RunWindow();

	HBITMAP CreateBmpHandler(BitmapFile* bmp);
	void SetCustomParam(WindowCustomParam* param);
	void CallRedraw();

private:
	HINSTANCE _hInstance;
	HWND _hWnd;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void RegisterWindowClass();
	HWND CreateMainWindow();
};

struct WindowCustomParam {
	BitmapFile* bmp;
	HBITMAP* hBmp;
};