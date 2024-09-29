#include "pch.h"

WindowHandler::WindowHandler(HINSTANCE hInstance) {
    _hInstance = hInstance;

    RegisterWindowClass();

    _hWnd = CreateMainWindow();
    if (_hWnd == NULL)
    {
        throw new std::exception("error in window creation");
    }

    /* Initalizes common controls */
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES; // Initialize tab control class
    InitCommonControlsEx(&icex);


    /* Initializes UI Elements */
    RECT rcApp;
    GetClientRect(_hWnd, &rcApp);
    _hTabControl = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, _globalPad, _globalPad, rcApp.right, rcApp.bottom, _hWnd, NULL, _hInstance, NULL);
    _TCIEncode.mask = TCIF_TEXT;

}

WindowHandler::~WindowHandler() {

}

void WindowHandler::RunWindow() {
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND WindowHandler::CreateMainWindow() {
    HWND hwnd = CreateWindowEx(
        0,
        L"MyWindowClass",
        L"My Win32 Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, 
        NULL,
        _hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOW);
    return hwnd;
}

void WindowHandler::RegisterWindowClass() {
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = _hInstance;
    wc.lpszClassName = L"MyWindowClass";

    RegisterClass(&wc);
}

LRESULT WindowHandler::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowCustomParam* customParam = (WindowCustomParam*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch (uMsg)
    {
    case WM_PAINT:
        if (customParam == nullptr) return NULL;
        // Need to define a scope otherwise compiler isn't happy with variable definition
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);

            HBITMAP hBmp = *customParam->hBmp;
            HDC memDC = CreateCompatibleDC(hDC);
            SelectObject(memDC, hBmp);

            //StretchBlt(hDC, 10, 10, 40, 40, memDC, 0, 0, customParam->bmp->_infoHeader->biHeight, customParam->bmp->_infoHeader->biWidth, SRCCOPY);
            BitBlt(hDC, 10, 10, customParam->bmp->_infoHeader->biWidth, customParam->bmp->_infoHeader->biHeight, memDC, 0, 0, SRCCOPY);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

HBITMAP WindowHandler::CreateBmpHandler(BitmapAtHome* bmp) {
    // Set that in it's own function rather than directly in the WinProc for performences reasons
    HDC hDC = GetDC(_hWnd);
    return  CreateDIBitmap(hDC, bmp->_infoHeader, CBM_INIT, bmp->_colorTable, (BITMAPINFO*)bmp->_infoHeader, DIB_RGB_COLORS);
}

void WindowHandler::SetCustomParam(WindowCustomParam* param) {
    SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(param));
}

void WindowHandler::CallRedraw() {
    RedrawWindow(_hWnd, 0, 0, RDW_INTERNALPAINT | RDW_INVALIDATE);
}
