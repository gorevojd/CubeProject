#ifndef WIN32_CODE_H

#include <Windows.h>

struct win32_screen_buffer{
	HWND Window;
	BITMAPINFO bmi;

	int Width;
	int Height;

	int fullscreenWidth;
	int fullscreenHeight;

	int currentWidth;
	int currentHeight;

	void* Memory;
	bool IsFullscreen;
};

struct window_dimension{
	int Width;
	int Height;
};

#define WIN32_CODE_H
#endif