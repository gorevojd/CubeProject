#ifndef WIN32_CODE_H

#include <Windows.h>
#include <dsound.h>

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

struct sound_buffer{
	LPDIRECTSOUNDBUFFER Buffer;
	DWORD Size;
	WORD Format;
	WORD Channels;
	DWORD SamplesPerSecond;
	DWORD AvgBytesPerSec;
	WORD BlockAlign;
	WORD BitsPerSample;
	DWORD RunningSample;
};

struct capture_buffer{
	LPDIRECTSOUNDCAPTUREBUFFER Buffer;
	DWORD Size;
	WORD Format;
	WORD Channels;
	DWORD SamplesPerSecond;
	DWORD AvgBytesPerSec;
	WORD BlockAlign;
	WORD BitsPerSample;
	DWORD RunningSample;
};


#define WIN32_CODE_H
#endif