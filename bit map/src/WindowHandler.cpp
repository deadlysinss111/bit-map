#include "pch.h"

/* Static fields definition */
WindowCustomParam WindowHandler::_cParam;
HWND** WindowHandler::_hENCODEarrElements = new HWND * [ENCODE_SCREEN_HWND_COUNT];
HWND** WindowHandler::_hDECODEarrElements = new HWND * [DECODE_SCREEN_HWND_COUNT];

WindowHandler::WindowHandler(HINSTANCE hInstance) {
    _hInstance = hInstance;

    RegisterWindowClass();

    _hWnd = CreateMainWindow();
    if (_hWnd == NULL) throw new std::exception("Failed to crete the main Window !");

    _hDc = GetDC(_hWnd);
    if (_hDc == NULL) throw new std::exception("Failed to retrieve the DC !");

    /*
       Prepares the static WindowCustomParam structure, used to call functions when message pumping
       - Sets a pointer to its own home class
       - The other 2 fields are set in main.cpp
    */
    _cParam.windowHandler = this;

    /*
       Initalizes common controls
    */
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES; // Initialize tab control class
    InitCommonControlsEx(&icex);

    /* Initializes UI Elements */
    CreateUIElements();

    /* Sets the app on the right tab */
    TabCtrl_SetCurSel(_hTabControl, ENCODE_TAB_INDEX);
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

LRESULT WindowHandler::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //if (_cParam.windowHandler == nullptr) std::cerr << "winH  ";
    //if (_cParam.bitmapAtHome == nullptr) std::cerr << "bmpH  ";
    //if (_cParam.hBmp == nullptr) std::cerr << "hBmp  ";

    //if (_cParam.windowHandler == nullptr || _cParam.bitmapAtHome == nullptr || _cParam.hBmp == nullptr)
    //{
    //    std::cerr << "are nullptr\n";
    //    return E_FAIL;
    //}

    if (_cParam.windowHandler == nullptr || _cParam.hBmp == nullptr)
    {
        std::cout << "It's too soon...\n";
        return E_FAIL;
    }

    switch (uMsg)
    {
    case WM_COMMAND:
        {
            int hiW = HIWORD(wParam);
            int loW = LOWORD(wParam);

            // Figure out which button sent the message
            switch (hiW)
            {
            case IDB_HOST:
                if (loW == BN_CLICKED)
                {
                    // Structure to open a file dialog box to retreve its path
                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = _cParam.windowHandler->_hWnd;
                    ofn.hInstance = _cParam.windowHandler->_hInstance;
                    ofn.lpstrFilter = L"BMP 24 bit Image\0*.bmp\0\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFile = _cParam.windowHandler->_hostFilePath;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                    if (!GetOpenFileName(&ofn)) std::cerr << "Couldn't open the prompt probably ? idk" << std::endl;
                    else std::wcout << "File path is " << _cParam.windowHandler->_hostFilePath << std::endl;
                }
                break;
            }
        }
        break;

    case WM_SIZE:
        _cParam.windowHandler->ResizeTabControl(lParam);
        break;

    case WM_NOTIFY:
        // Switching on the code of the WM_NOTIFY Message
        switch (((LPNMHDR)lParam)->code)
        {
            // We let the user change tab
        case TCN_SELCHANGING:
            return FALSE;
            break;

            // We load in the correct UI elements to show the correct screen
        case TCN_SELCHANGE:
            switch (TabCtrl_GetCurSel(_cParam.windowHandler->_hTabControl))
            {
            case ENCODE_TAB_INDEX:
                SendMessage(_cParam.windowHandler->_hWnd, WM_SETTEXT, 0, (LPARAM) & (_cParam.windowHandler->_encodeTabName));
                // Make the UI elements of the Encode screen visible
                for (int i = 0; i < ENCODE_SCREEN_HWND_COUNT; ++i)
                {
                    ShowWindow(*_hENCODEarrElements[i], SW_SHOW);
                    continue;
                }

                // Hides the UI elements of the Decode screen
                for (int i = 0; i < DECODE_SCREEN_HWND_COUNT; ++i)
                {
                    ShowWindow(*_hDECODEarrElements[i], SW_HIDE);
                    continue;
                }

                std::cout << "Sel Change to ENCODE finished" << std::endl;

                break;
            case DECODE_TAB_INDEX:
                SendMessage(_cParam.windowHandler->_hWnd, WM_SETTEXT, 0, (LPARAM) & (_cParam.windowHandler->_decodeTabName));
                // Make the UI elements of the Decode screen visible
                for (int i = 0; i < DECODE_SCREEN_HWND_COUNT; ++i)
                {
                    ShowWindow(*_hDECODEarrElements[i], SW_SHOW);
                    continue;
                }

                // Hides the UI elements of the Encode screen              
                for (int i = 0; i < ENCODE_SCREEN_HWND_COUNT; ++i)
                {
                    ShowWindow(*_hENCODEarrElements[i], SW_HIDE);
                    continue;
                }

                std::cout << "Sel Change to DECODE finished" << std::endl;

                break;
            }
            break;
        }
        break;

    case WM_PAINT:
        // Need to define a scope otherwise compiler isn't happy with variable definition
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        //if (_cParam.hBmp != nullptr) {
        //    HDC hDC = BeginPaint(_cParam.windowHandler->_hENCODEbtnOpenHostBMP, &ps);
        //    HDC memDC = CreateCompatibleDC(hDC);
        //    SelectObject(memDC, *(_cParam.hBmp));

        //    //StretchBlt(hDC, 10, 10, 40, 40, memDC, 0, 0, customParam->bmp->_infoHeader->biHeight, customParam->bmp->_infoHeader->biWidth, SRCCOPY);
        //    BitBlt(hDC, 100, 100, _cParam.img->_width, _cParam.img->_height, memDC, 0, 0, SRCCOPY);

        //    //RedrawWindow(_cParam.windowHandler->_hENCODEbtnOpenHostBMP, 0, 0, RDW_INTERNALPAINT | RDW_INVALIDATE);
        //    SetWindowPos(_cParam.windowHandler->_hENCODEbtnOpenHostBMP, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        //}
        

        
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

HBITMAP WindowHandler::CreateBmpHandler(BitmapFile* bmp) {
    // Set that in it's own function rather than directly in the WinProc for performences reasons
    return  CreateDIBitmap(_hDc, bmp->_infoHeader, CBM_INIT, bmp->_pixelData, (BITMAPINFO*)bmp->_infoHeader, DIB_RGB_COLORS);
}

HBITMAP WindowHandler::CreateBmpHandler(RawFile* bmp) {
    BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(bmp->_buffer + sizeof(BITMAPFILEHEADER));
    BYTE* pixelData = bmp->_buffer + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // Set that in it's own function rather than directly in the WinProc for performences reasons
    return  CreateDIBitmap(_hDc, infoHeader, CBM_INIT, pixelData, (BITMAPINFO*)infoHeader, DIB_RGB_COLORS);
}

void WindowHandler::CallRedraw() {
    RedrawWindow(_hWnd, 0, 0, RDW_INTERNALPAINT | RDW_INVALIDATE);
}

/* UI specific functions */
// Resizes the Tab Control to match the new window's size
HRESULT WindowHandler::ResizeTabControl(LPARAM ARGlParam)
{
    RECT rcApp;
    GetClientRect(_hWnd, &rcApp);
    if (!SetWindowPos(_hTabControl, HWND_TOP, _globalPad, _globalPad, LOWORD(ARGlParam), HIWORD(ARGlParam), SWP_SHOWWINDOW)) {
        std::cerr << "Failed to resize the Tab Control !" << std::endl;
        return E_FAIL;
    }

    return S_OK;
}

// Creates every UI elements (should be called on construction)
void WindowHandler::CreateUIElements()
{
    /*
       Initializes UI Elements : Tabs
       - Creates the Tab Control object
       - Creates both Tab Items
       - Inserts both Tab Items
    */
    RECT rcTemp;    // Used to positon windows created here relatively to each other
    GetClientRect(_hWnd, &rcTemp);
    _hTabControl = CreateWindow(
        WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rcTemp.right, rcTemp.bottom,
        _hWnd, NULL, _hInstance, NULL);
    if (_hTabControl == NULL) std::cerr << "Creation of the Tab Control failed and is NULL !";

    TCITEM TCIEncode;
    TCIEncode.mask = TCIF_TEXT;
    TCIEncode.pszText = _encodeTabName;
    TCIEncode.cchTextMax = sizeof(_encodeTabName);
    TCITEM TCIDecode;
    TCIDecode.mask = TCIF_TEXT;
    TCIDecode.pszText = _decodeTabName;
    TCIDecode.cchTextMax = sizeof(_decodeTabName);

    if (TabCtrl_InsertItem(_hTabControl, ENCODE_TAB_INDEX, &TCIEncode) == -1) {
        std::cerr << "Insertion of TCIEncode failed ! Destroying Tab Control..." << std::endl;
        DestroyWindow(_hTabControl);
    }
    if (TabCtrl_InsertItem(_hTabControl, DECODE_TAB_INDEX, &TCIDecode) == -1) {
        std::cerr << "Insertion of TCIDecode failed ! Destroying Tab Control..." << std::endl;
        DestroyWindow(_hTabControl);
    }

    /*
       Initializes UI Elements : Encoding Screen
       - Creates the button for the host
       - Creates the static window for the .bmp preview
       - Creates the button for the parasite
       - Creates an edit control to type a message to inject in
       - Creates a static text more to the right
       - Creates the static window for the parsited .bmp preview
       - Creates the button to inject the parasite file
       - Creates the button to inject the parasite message
    */

    GetClientRect(_hWnd, &rcTemp);
    TabCtrl_AdjustRect(_hTabControl, FALSE, &rcTemp);
    _hENCODEbtnOpenHostBMP = CreateWindow(
        WC_BUTTON, L"Open a host bitmap image...", BUTTON_STYLE,
        rcTemp.left + _globalPad, rcTemp.top + _globalPad, 200, 20,
        _hWnd, (HMENU)IDB_HOST, _hInstance, NULL);
    if (_hENCODEbtnOpenHostBMP == NULL) std::cerr << "Creation of the Host Button failed and is NULL !";
    _hENCODEarrElements[0] = &_hENCODEbtnOpenHostBMP;

    GetWindowRect(_hENCODEbtnOpenHostBMP, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEstaticHostPreview = CreateWindow(
        WC_STATIC, L"", STATIC_FRAME_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, BMP_PREVIEW_DIMS, BMP_PREVIEW_DIMS,
        _hWnd, NULL, _hInstance, NULL);
    if (_hENCODEstaticHostPreview == NULL) std::cerr << "Creation of the Static Host Preview failed and is NULL !";
    _hENCODEarrElements[1] = &_hENCODEstaticHostPreview;

    GetWindowRect(_hENCODEstaticHostPreview, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEbtnOpenParasiteFile = CreateWindow(
        WC_BUTTON, L"Open a file to inject in...", BUTTON_STYLE,
        rcTemp.left, rcTemp.bottom + _strongPad, 200, 20,
        _hWnd, (HMENU)IDB_PARASITE, _hInstance, NULL);
    if (_hENCODEbtnOpenParasiteFile == NULL) std::cerr << "Creation of the Parasite Button failed and is NULL !";
    _hENCODEarrElements[2] = &_hENCODEbtnOpenParasiteFile;

    GetWindowRect(_hENCODEbtnOpenParasiteFile, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEeditParasiteMessage = CreateWindow(
        WC_EDIT, L"If not a file, type your injected message here.", EDIT_STYLE,
        rcTemp.left - _smolPad, rcTemp.bottom + _globalPad, 200 + _smolPad, 100,
        _hWnd, (HMENU)IDE_PARASITE, _hInstance, NULL);
    if (_hENCODEeditParasiteMessage == NULL) std::cerr << "Creation of the Edit Parasite failed and is NULL !";
    _hENCODEarrElements[3] = &_hENCODEeditParasiteMessage;

    GetWindowRect(_hENCODEbtnOpenHostBMP, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEstaticResultTitle = CreateWindow(
        WC_STATIC, L"Preview of the result of the injection :", STATIC_TEXT_STYLE,
        rcTemp.right + _strongPad, rcTemp.top, 400, 15,
        _hWnd, NULL, _hInstance, NULL);
    if (_hENCODEstaticResultTitle == NULL) std::cerr << "Creation of the Static Result Title failed and is NULL !";
    _hENCODEarrElements[4] = &_hENCODEstaticResultTitle;

    GetWindowRect(_hENCODEstaticResultTitle, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEstaticResultPreview = CreateWindow(
        WC_STATIC, L"", STATIC_FRAME_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, BMP_PREVIEW_DIMS * 2, BMP_PREVIEW_DIMS * 2,
        _hWnd, NULL, _hInstance, NULL);
    if (_hENCODEstaticResultPreview == NULL) std::cerr << "Creation of the Static Result Preview failed and is NULL !";
    _hENCODEarrElements[5] = &_hENCODEstaticResultPreview;

    GetWindowRect(_hENCODEstaticResultPreview, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hENCODEbtnInjectFile = CreateWindow(
        WC_BUTTON, L"Inject file", BUTTON_STYLE,
        rcTemp.left + 2 * _strongPad, rcTemp.bottom + _strongPad, 130, 20,
        _hWnd, (HMENU)IDB_INJECT_FILE, _hInstance, NULL);
    if (_hENCODEbtnInjectFile == NULL) std::cerr << "Creation of the Injectf File Button failed and is NULL !";
    _hENCODEarrElements[6] = &_hENCODEbtnInjectFile;

    _hENCODEbtnInjectMessage = CreateWindow(
        WC_BUTTON, L"Inject message", BUTTON_STYLE,
        rcTemp.right - 130 - 2 * _strongPad, rcTemp.bottom + _strongPad, 130, 20,
        _hWnd, (HMENU)IDB_INJECT_TEXT, _hInstance, NULL);
    if (_hENCODEbtnInjectMessage == NULL) std::cerr << "Creation of the Inject Message Button failed and is NULL !";
    _hENCODEarrElements[7] = &_hENCODEbtnInjectMessage;

    /*
       Initializes UI Elements : Decoding Screen
       - Creates the button for the target
       - Creates the static window for the .bmp preview
       - Creates the static text to know where errors would go
       - Creates the edit (unwrittable) to display the operation status in
       - Creates a static text more to the right
       - Creates the edit (unwrittable) where the decoded message will go
       - Creates the button to extract the message
    */

    GetClientRect(_hWnd, &rcTemp);
    TabCtrl_AdjustRect(_hTabControl, FALSE, &rcTemp);
    _hDECODEbtnOpenInfectedBMP = CreateWindow(
        WC_BUTTON, L"Open an infected bitmap image...", BUTTON_STYLE,
        rcTemp.left + _globalPad, rcTemp.bottom + _globalPad, 200, 20,
        _hWnd, (HMENU)IDB_INFECTED, _hInstance, NULL);
    if (_hDECODEbtnOpenInfectedBMP == NULL) std::cerr << "Creation of the Open Infected Button failed and is NULL !";
    _hDECODEarrElements[0] = &_hDECODEbtnOpenInfectedBMP;

    GetWindowRect(_hDECODEbtnOpenInfectedBMP, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEstaticInfectedPreview = CreateWindow(
        WC_STATIC, L"", STATIC_FRAME_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, BMP_PREVIEW_DIMS, BMP_PREVIEW_DIMS,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEstaticInfectedPreview == NULL) std::cerr << "Creation of the Static Infected Preview failed and is NULL !";
    _hDECODEarrElements[1] = &_hDECODEstaticInfectedPreview;

    GetWindowRect(_hDECODEstaticInfectedPreview, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEstaticOperationResult = CreateWindow(
        WC_STATIC, L"Extraction result :", STATIC_TEXT_STYLE,
        rcTemp.left, rcTemp.bottom + 5 * _strongPad, 200, 15,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEstaticOperationResult == NULL) std::cerr << "Creation of the Extraction Result Title failed and is NULL !";
    _hDECODEarrElements[2] = &_hDECODEstaticOperationResult;

    GetWindowRect(_hDECODEstaticOperationResult, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEeditOperationResult = CreateWindow(
        WC_EDIT, L"", FIXED_EDIT_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, 200, 70,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEeditOperationResult == NULL) std::cerr << "Creation of the Extraction Result Text failed and is NULL !";
    _hDECODEarrElements[3] = &_hDECODEeditOperationResult;

    GetWindowRect(_hDECODEbtnOpenInfectedBMP, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEstaticMessageTitle = CreateWindow(
        WC_STATIC, L"Extracted data from the infected file :", STATIC_TEXT_STYLE,
        rcTemp.right + _strongPad, rcTemp.top, 200, 15,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEstaticOperationResult == NULL) std::cerr << "Creation of the Static Message Title failed and is NULL !";
    _hDECODEarrElements[4] = &_hDECODEstaticMessageTitle;
    std::cout << "DECODE[4] : " << _hDECODEarrElements[4] << std::endl;

    GetWindowRect(_hDECODEstaticMessageTitle, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEeditExtractedData = CreateWindow(
        WC_EDIT, L"", FIXED_EDIT_STYLE,
        rcTemp.left, rcTemp.bottom + _globalPad, 300, 170,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEeditExtractedData == NULL) std::cerr << "Creation of the Extracted Data Area failed and is NULL !";
    _hDECODEarrElements[5] = &_hDECODEeditExtractedData;
    std::cout << "DECODE[5] : " << _hDECODEarrElements[5] << std::endl;

    GetWindowRect(_hDECODEeditExtractedData, &rcTemp);
    MapWindowPoints(NULL, _hTabControl, (LPPOINT)&rcTemp, 2);
    _hDECODEbtnExtractData = CreateWindow(
        WC_BUTTON, L"Extract Data", BUTTON_STYLE,
        rcTemp.left + rcTemp.right / 2, rcTemp.bottom + _strongPad, 200, 20,
        _hWnd, NULL, _hInstance, NULL);
    if (_hDECODEbtnExtractData == NULL) std::cerr << "Creation of the Extract Data Button failed and is NULL !";
    _hDECODEarrElements[6] = &_hDECODEbtnExtractData;
    std::cout << "DECODE[6] : " << _hDECODEarrElements[6] << std::endl;
    std::cout << "Last Error : " << GetLastError();
}