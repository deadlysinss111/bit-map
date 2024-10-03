#include "pch.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Instantiate a window
	WindowHandler handler(hInstance);

	// Starts the Message Pumping process, ending the program once the window is closed
	handler.RunWindow();
}