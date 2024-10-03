#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

// For printing stuff
#include <string>
#include <bitset>
#include <cassert>

#include <Windows.h>
#include <wingdi.h>

// Used for Common Controls, mainly tabs
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class BitmapToolbox;
class WindowHandler;
class RawFile;
class BitmapFile;
class CImage;
class CImageToolbox;

struct WindowCustomParam;
struct CustomHeader;



#include "BitmapToolbox.h"
#include "WindowHandler.h"
#include "RawFile.h"
#include "BitmapFile.h"
#include "CImage.h"
#include "CImageToolbox.h"