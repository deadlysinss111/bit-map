#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include <wingdi.h>

class BitmapAtHome;
class InfectedBitmap;
class WindowHandler;

struct WindowCustomParam;
struct CustomHeader;

#define CUSTOMHEADERSIZE = 80

#include "BitmapAtHome.h"
#include "InfectedBitmap.h"
#include "WindowHandler.h"