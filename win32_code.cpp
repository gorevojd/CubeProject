#include "platform_code.h"
#include <GL/glew.h>
#include <GL/wglew.h>

#include <mmsystem.h>
#include <Windows.h>
#include <dsound.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "win32_code.h"

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPGUID, LPDIRECTSOUND*, LPUNKNOWN)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
DIRECT_SOUND_CREATE(DirectSoundCreateStub){
    return 0;
}
static direct_sound_create* DirectSoundCreate_ = DirectSoundCreateStub;
#define DirectSoundCreate DirectSoundCreate_

#define DIRECT_SOUND_CAPTURE_CREATE(name) HRESULT WINAPI name(LPGUID, LPDIRECTSOUNDCAPTURE*, LPUNKNOWN)
typedef DIRECT_SOUND_CAPTURE_CREATE(direct_sound_capture_create);
DIRECT_SOUND_CAPTURE_CREATE(DirectSoundCaptureCreateStub){
    return 0;
}
static direct_sound_capture_create* DirectSoundCaptureCreate_ = DirectSoundCaptureCreateStub;
#define DirectSoundCaptureCreate DirectSoundCaptureCreate_

static void InitOpenGL(win32_screen_buffer* buffer, HINSTANCE Instance);
static character_atlas Win32LoadTrueTypeFontToAtlas(const char* fontFilePath, u32 pixelHeight = 48);
static void InitOpenGL(win32_screen_buffer* buffer, HINSTANCE Instance);
static void Win32InitScreenBuffer(win32_screen_buffer* Buffer, int width, int height);
void ProcessGameButtonState(game_button_state* State, bool IsDown, bool WasDown);
void ProcessPendingMessagesToController(int cid);
static void Win32RecreateWindow(win32_screen_buffer* Buffer, HINSTANCE Instance, const char* windowName, int windowWidth, int windowHeight);

static bool GlobalRunning = false;
static win32_screen_buffer GlobalScreen;
static game_input* GlobalGameInput = new game_input();
static game_camera* GlobalGameCamera = new game_camera();
static game_time GlobalTime;
static game_memory GlobalGameMemory;
static character_info* characters = new character_info[128];
static character_atlas GlobalCharacterAtlas;
static WINDOWPLACEMENT GlobalWindowPosition;

sound_buffer Win32InitDirectSound(HWND window){
    HINSTANCE DSoundLib = LoadLibraryA("dsound.dll");
    direct_sound_create* DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLib, "DirectSoundCreate");

    sound_buffer result = {};
    
    LPDIRECTSOUND DirectSound;
    if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))){
        WAVEFORMATEX WaveFormat;
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat.nChannels = 2;
        WaveFormat.nSamplesPerSec = 44100;
        WaveFormat.wBitsPerSample = 16;
        WaveFormat.nBlockAlign = 4;//2 * 16 / 8
        WaveFormat.nAvgBytesPerSec = 44100 * 4;
        WaveFormat.cbSize = sizeof(WAVEFORMATEX);

        if (SUCCEEDED(DirectSound->SetCooperativeLevel(window, DSSCL_PRIORITY))){
            DSBUFFERDESC Description;
            memset(&Description, 0, sizeof(DSBUFFERDESC));
            Description.dwSize = sizeof(DSBUFFERDESC);
            Description.dwFlags = DSBCAPS_PRIMARYBUFFER;
            Description.dwBufferBytes = 0;
            Description.dwReserved = 0;
            Description.guid3DAlgorithm = GUID_NULL;
            Description.lpwfxFormat = 0;

            LPDIRECTSOUNDBUFFER PrimaryBuffer;
            if (SUCCEEDED(DirectSound->CreateSoundBuffer(&Description, &PrimaryBuffer, 0))){
                PrimaryBuffer->SetFormat(&WaveFormat);
            }
            else{
                //TODO(Dima): Logging
            }
        }
        else{
            //TODO(Dima): Logging
        }

        DSBUFFERDESC Desc;
        memset(&Desc, 0, sizeof(DSBUFFERDESC));
        Desc.dwSize = sizeof(DSBUFFERDESC);
        Desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
        Desc.dwBufferBytes = WaveFormat.nAvgBytesPerSec * 4; // 4 second lenght buffer
        Desc.lpwfxFormat = &WaveFormat;
        Desc.dwReserved = 0;
        Desc.guid3DAlgorithm = GUID_NULL;

        LPDIRECTSOUNDBUFFER SecondaryBuffer;
        if (SUCCEEDED(DirectSound->CreateSoundBuffer(&Desc, &SecondaryBuffer, 0))){
            result.Buffer = SecondaryBuffer;
            result.Format = WaveFormat.wFormatTag;
            result.AvgBytesPerSec = WaveFormat.nAvgBytesPerSec;
            result.BitsPerSample = WaveFormat.wBitsPerSample;
            result.Channels = WaveFormat.nChannels;
            result.Size = Desc.dwBufferBytes;
            result.SamplesPerSecond = WaveFormat.nSamplesPerSec;
            result.RunningSample = 0;
            return result;
        }
        else{
            //TODO(Dima): Logging
        }
    }
    else{
        //TODO(Dima): Logging
    }

    return result;
}

capture_buffer win32InitDirectSoundCapture()
{
    HINSTANCE DSoundLib = LoadLibraryA("dsound.dll");
    direct_sound_capture_create* DirectSoundCaptureCreate =
        (direct_sound_capture_create*)GetProcAddress(DSoundLib, "DirectSoundCaptureCreate");

    capture_buffer result = {};
    
    LPDIRECTSOUNDCAPTURE DirectSoundCapture;
    HRESULT CaptureCreateResult = DirectSoundCaptureCreate(0, &DirectSoundCapture, 0);
    if (DirectSoundCaptureCreate && SUCCEEDED(CaptureCreateResult))
    {
        WAVEFORMATEX WaveFormat = {};
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat.nChannels = 2;
        WaveFormat.nSamplesPerSec = 44100;
        WaveFormat.wBitsPerSample = 16;
        WaveFormat.nBlockAlign = 4;
        WaveFormat.nAvgBytesPerSec = 44100 * 4;
        WaveFormat.cbSize = 0;

        DSCBUFFERDESC CaptureBufferDescription = {};
        CaptureBufferDescription.dwSize = sizeof(DSCBUFFERDESC);
        CaptureBufferDescription.dwFlags = 0;
        CaptureBufferDescription.dwBufferBytes = WaveFormat.nAvgBytesPerSec * 4;
        CaptureBufferDescription.dwReserved = 0;
        CaptureBufferDescription.lpwfxFormat = &WaveFormat;
        CaptureBufferDescription.dwFXCount = 0;
        CaptureBufferDescription.lpDSCFXDesc = NULL;

        LPDIRECTSOUNDCAPTUREBUFFER SecondaryBuffer;
        if (SUCCEEDED(DirectSoundCapture->CreateCaptureBuffer(
            &CaptureBufferDescription,
            &SecondaryBuffer, 0)))
        {
            result.Buffer = SecondaryBuffer;
            result.AvgBytesPerSec = WaveFormat.nAvgBytesPerSec;
            result.BitsPerSample = WaveFormat.wBitsPerSample;
            result.BlockAlign = WaveFormat.nBlockAlign;
            result.Channels = WaveFormat.nChannels;
            result.Format = WaveFormat.wFormatTag;
            result.SamplesPerSecond = WaveFormat.nSamplesPerSec;
            result.Size = WaveFormat.nAvgBytesPerSec * 4; // 1 second lenght
            return result;
        }
    }
    else{
        //TODO(Dima): Logging
        /*
        if (CaptureCreateResult == DSERR_INVALIDPARAM){
            OutputDebugStringA("DSCaptureCreate Error: DSERR_INVALIDPARAM\n");
        }
        if (CaptureCreateResult == DSERR_NOAGGREGATION){
            OutputDebugStringA("DSCaptureCreate Error: DSERR_NOAGGREGATION\n");
        }
        if (CaptureCreateResult == DSERR_OUTOFMEMORY){
            OutputDebugStringA("DSCaptureCreate Error: DSERR_OUTOFMEMORY\n");
        }
        */
    }

    return result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
    bool32 Result = false;

    HANDLE FileHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE){
        DWORD BytesWritten;
        if (WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0)){
            // NOTE(Dima): File read successfully
            Result = (BytesWritten == MemorySize);
        }
        else{
            // TODO(Dima): Logging
        }

        CloseHandle(FileHandle);
    }
    else{
        // TODO(Dima): Logging
    }

    return(Result);
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile){
    debug_read_file_result Result = {};
    HANDLE handle = CreateFileA(
        fileName,
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (handle != INVALID_HANDLE_VALUE){
        LARGE_INTEGER FileSize;
        GetFileSizeEx(handle, &FileSize);
        void* ResultMemory = VirtualAlloc(0, FileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        DWORD BytesRead;
        if (ReadFile(handle, ResultMemory, FileSize.QuadPart, &BytesRead, 0)){
            CloseHandle(handle);
            char str[10];
            _itoa_s(BytesRead, str, 10);
            //OutputDebugStringA("Bytes read: ");
            //OutputDebugStringA(str);
            //OutputDebugStringA("\n");
            Result.contents = ResultMemory;
            Result.size = FileSize.QuadPart;
            return Result;
        }
        else{
            //TODO(Dima): Logging
        }
    }
    else{
        //TODO(Dima): Logging
    }
    return Result;
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory){
    if (Memory != NULL){
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}


static void Win32LoadTrueTypeFont(const char* fontFilePath, u32 pixelHeight){
    FT_Library ft;
    if (FT_Init_FreeType(&ft)){
        OutputDebugStringA("Can not init FreeType library. \n");
    }
    else{

        FT_Face face;
        if (FT_New_Face(ft, fontFilePath, 0, &face)){
            OutputDebugStringA("Can not load font.\n");
        }
        else{
            FT_Set_Pixel_Sizes(face, 0, pixelHeight);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            for (int i = 0; i < 128; i++){
                if (FT_Load_Char(face, i, FT_LOAD_RENDER)){
                    OutputDebugStringA("Failed to load glyph.\n");
                    continue;
                }
                else{
                    GLuint texture;
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_ALPHA,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        0,
                        GL_ALPHA,
                        GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    character_info newChar = {
                        texture,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        face->glyph->bitmap_left,
                        face->glyph->bitmap_top,
                        face->glyph->advance.x
                    };
                    characters[i] = newChar;
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }
            FT_Done_Face(face);
            FT_Done_FreeType(ft);
        }
    }
}

static character_atlas Win32LoadTrueTypeFontToAtlas(const char* fontFilePath, u32 pixelHeight){

    character_atlas* retVal = new character_atlas[128];

    FT_Library ft;
    if (FT_Init_FreeType(&ft)){
        OutputDebugStringA("Can not init FreeType library. \n");
    }
    else{
        u32 atlasWidth = 0;
        u32 atlasHeight = 0;
        FT_Face face;
        if (FT_New_Face(ft, fontFilePath, 0, &face)){
            OutputDebugStringA("Can not load font.\n");
        }
        else{
            FT_Set_Pixel_Sizes(face, 0, pixelHeight);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            for (int i = 0; i < 128; i++){
                if (FT_Load_Char(face, i, FT_LOAD_RENDER)){
                    OutputDebugStringA("Failed to load glyph.\n");
                    continue;
                }
                else{
                    atlasWidth += face->glyph->bitmap.width;
                    atlasHeight = max(atlasHeight, face->glyph->bitmap.rows);
                }
            }

            GLuint atlasTexture;
            glGenTextures(1, &atlasTexture);
            glBindTexture(GL_TEXTURE_2D, atlasTexture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_ALPHA,
                atlasWidth,
                atlasHeight,
                0,
                GL_ALPHA,
                GL_UNSIGNED_BYTE,
                0);

            u32 offsetX = 0;
            for (int i = 0; i < 128; i++){
                if (FT_Load_Char(face, i, FT_LOAD_RENDER)){
                    OutputDebugStringA("Failed to load glyph.\n");
                    continue;
                }
                else{

                    glTexSubImage2D(
                        GL_TEXTURE_2D,
                        0,
                        offsetX,
                        0,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        GL_ALPHA,
                        GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer);

                    atlas_character_info atlchi = {};
                    atlchi.AdvanceX = face->glyph->advance.x;
                    atlchi.AdvanceY = face->glyph->advance.y;
                    atlchi.BearingX = face->glyph->bitmap_left;
                    atlchi.BearingY = face->glyph->bitmap_top;
                    atlchi.Width = face->glyph->bitmap.width;
                    atlchi.Height = face->glyph->bitmap.rows;
                    atlchi.tCoordsX1 = (float)offsetX / (float)atlasWidth;
                    atlchi.tCoordsX2 = (float)(offsetX + face->glyph->bitmap.width) / (float)atlasWidth;
                    atlchi.tCoordsY1 = 1.0f - (float)face->glyph->bitmap.rows / (float)atlasHeight;
                    atlchi.tCoordsY2 = 1.0f;
                    retVal->Characters[i] = atlchi;

                    offsetX += face->glyph->bitmap.width;
                }
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            retVal->TextureID = atlasTexture;
            retVal->Width = atlasWidth;
            retVal->Height = atlasHeight;

            glBindTexture(GL_TEXTURE_2D, 0);

            FT_Done_Face(face);
            FT_Done_FreeType(ft);

            return *retVal;
        }
    }
    return *retVal;
}

void Win32InitGameTime(game_time& appTime){
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    appTime.time = 0.0f;
    appTime.deltaTime = 0.0f;
    appTime.fps = 0.0f;
    appTime.freq = 0.0f;
    appTime.count = 0.0f;
    appTime.freq = freq.QuadPart;
}

void ProcessBeginOfFrameTime(game_time& appTime){
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    appTime.count = count.QuadPart;
}

void ProcessEndOfFrameTime(game_time& appTime){
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    LONGLONG newCount = count.QuadPart;
    LONGLONG cyclesElapsed = newCount - appTime.count;
    float deltaTime = (float)((float)cyclesElapsed / (float)appTime.freq);
    appTime.deltaTime = deltaTime;
    appTime.time += deltaTime;
    appTime.fps = 1.0f / deltaTime;
}



static void InitOpenGL(win32_screen_buffer* buffer, HINSTANCE Instance, bool sampling, int numberOfSamples = 4){
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                        //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    HDC hdc = GetDC(buffer->Window);
    int choosePFDresult = ChoosePixelFormat(hdc, &pfd);
    bool setPFres = SetPixelFormat(hdc, choosePFDresult, &pfd);

    HGLRC OpenGLRenderingContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, OpenGLRenderingContext);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK){
        OutputDebugStringA("Can not initialize glew.\n");
    }
    ReleaseDC(buffer->Window, hdc);


    //My mistake was is that is tried to get DC from the same window as before.
    //But SetPixelFormat wants us to pass DC that we never SetPixelFormat before.

    if (sampling == true){

        Win32RecreateWindow(buffer, Instance, "CubeProject", buffer->Width, buffer->Height);
        HDC tempDC = GetDC(buffer->Window);
        int pixelFormatMultisample;
        int pixelFormatsMultisample[10];
        UINT numFormats;

        float pfAttribFList[] = { 0, 0 };
        int piAttribList[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_SAMPLE_BUFFERS_ARB, 1, //Number of buffers (must be 1 at time of writing)
            WGL_SAMPLES_ARB, numberOfSamples,        //Number of samples
            0
        };

        bool valid = wglChoosePixelFormatARB(tempDC, piAttribList, pfAttribFList, 1, &pixelFormatMultisample, &numFormats);
        if (valid && numFormats >= 1){
            wglMakeCurrent(0, 0);
            wglDeleteContext(OpenGLRenderingContext);
            PIXELFORMATDESCRIPTOR pfd2 = {};
            //DescribePixelFormat(tempDC, pixelFormatMultisample, sizeof(pfd2), &pfd2);
            bool c = SetPixelFormat(tempDC, pixelFormatMultisample, &pfd2);
            DWORD er = GetLastError();
            HGLRC multisampleContext = wglCreateContext(tempDC);
            wglMakeCurrent(tempDC, multisampleContext);
        }
        glEnable(GL_MULTISAMPLE);

        ReleaseDC(buffer->Window, tempDC);
    }


    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    /*
    GL_ALWAYS The depth test always passes.
    GL_NEVER The depth test never passes.
    GL_LESS Passes if the fragment�s depth value is less than the stored depth value.
    GL_EQUAL Passes if the fragment�s depth value is equal to the stored depth value.
    GL_LEQUAL   Passes if the fragment�s depth value is less than or equal to the stored depth value.
	GL_GREATER Passes if the fragment�s depth value is greater than the stored depth value.
	GL_NOTEQUAL Passes if the fragment�s depth value is not equal to the stored depth value.
	GL_GEQUAL Passes if the fragment�s depth value is greater than or equal to the stored dept value.
	*/

	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_EQUAL, 1, 0xFF);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_FRAMEBUFFER_SRGB);

#if 1
    if (wglSwapIntervalEXT(2))
    {
        OutputDebugStringA("Swap interval has been set.\n");
    }
#endif
}


static void
Win32ToggleFullscreen(HWND Window)
{

    //MONITORINFO monInfo;
    //GetMonitorInfo(MonitorFromWindow(globalScreen->window, MONITOR_DEFAULTTOPRIMARY), &monInfo);
    //int w, h;
    //w = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
    //h = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;
    //SetWindowPos(window, HWND_TOPMOST, 0, 0, w, h, SWP_NOREDRAW);

    // NOTE(casey): This follows Raymond Chen's prescription
    // for fullscreen toggling, see:
    // http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx

    if (GlobalScreen.IsFullscreen == false){
        DWORD Style = GetWindowLong(Window, GWL_STYLE);
        if (Style & WS_OVERLAPPEDWINDOW)
        {
            MONITORINFO MonitorInfo = { sizeof(MonitorInfo) };
            if (GetWindowPlacement(Window, &GlobalWindowPosition) &&
                GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
            {
                SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(Window, HWND_TOP,
                    MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                    MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                    MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                    SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
            glViewport(0, 0,
                MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top);
            GlobalScreen.currentWidth = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
            GlobalScreen.currentHeight = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;
            GlobalScreen.IsFullscreen = !GlobalScreen.IsFullscreen;
        }
        else
        {
            MONITORINFO monInfo;
            GetMonitorInfo(MonitorFromWindow(GlobalScreen.Window, MONITOR_DEFAULTTOPRIMARY), &monInfo);
            int w, h;
            w = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
            h = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;

            GlobalScreen.currentWidth = w;
            GlobalScreen.currentHeight = h;

            SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
            SetWindowPlacement(Window, &GlobalWindowPosition);
            SetWindowPos(Window, 0, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        }
    }
    else{
        DWORD Style = GetWindowLong(Window, GWL_STYLE);
        float Width;
        float Height;
        if (!(Style & WS_OVERLAPPEDWINDOW)){
            SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
            Width = GlobalScreen.Width;
            Height = GlobalScreen.Height;
            SetWindowPos(Window, HWND_TOP, 100, 100, Width, Height,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
        glViewport(0, 0, Width, Height);
        GlobalScreen.currentWidth = Width;
        GlobalScreen.currentHeight = Height;
        GlobalScreen.IsFullscreen = !GlobalScreen.IsFullscreen;
    }
}

static void
Win32InitScreenBuffer(win32_screen_buffer* Buffer, int width, int height)
{
    if (Buffer->Memory){
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Buffer->bmi.bmiHeader.biWidth = width;
    Buffer->bmi.bmiHeader.biHeight = -height;
    Buffer->bmi.bmiHeader.biPlanes = 1;
    Buffer->bmi.bmiHeader.biBitCount = 32;
    Buffer->bmi.bmiHeader.biCompression = BI_RGB;
    Buffer->bmi.bmiHeader.biSizeImage = 0;

    int BytesPerPixel = 4;
    DWORD bufferSize = width * height * BytesPerPixel;
    //Buffer->Memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    MONITORINFO monInfo;
    GetMonitorInfo(MonitorFromWindow(Buffer->Window, MONITOR_DEFAULTTOPRIMARY), &monInfo);
    int w, h;
    w = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
    h = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;

    Buffer->Memory = 0;
    Buffer->Width = width;
    Buffer->Height = height;
    Buffer->currentWidth = width;
    Buffer->currentHeight = height;
    Buffer->fullscreenWidth = w;
    Buffer->fullscreenHeight = h;
} 

void ProcessGameButtonState(game_button_state* State, bool IsDown, bool WasDown){
    State->IsDown = IsDown;
    State->WasDown = WasDown;
}

void ProcessPendingMessagesToController(int cid){
    MSG Message;
    while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)){
        switch (Message.message){

        case WM_MOUSEMOVE:{
            //POINT MouseP;
            //GetCursorPos(&MouseP);

            //GlobalGameInput->controllers[cid].DeltaMouseX = MouseP.x - GlobalGameInput->controllers[cid].MouseX;
            //GlobalGameInput->controllers[cid].DeltaMouseY = MouseP.y - GlobalGameInput->controllers[cid].MouseY;

            //GlobalGameInput->controllers[cid].MouseX = MouseP.x;
            //GlobalGameInput->controllers[cid].MouseY = MouseP.y;

            //SetCursorPos(GlobalScreen.Width / 2, GlobalScreen.Height/ 2);
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:{
            WPARAM vkey = Message.wParam;
            bool WasDown = ((Message.lParam & (1 << 30)) != 0);
            bool IsDown = ((Message.lParam & (1 << 31)) == 0);
            bool AltKeyWasDown = ((Message.lParam & (1 << 29)) != 0);
            if (WasDown != IsDown){
                switch (vkey){
                case 'A':{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].LeftKey, IsDown, WasDown);
                }break;
                case 'W':{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].UpKey, IsDown, WasDown);
                }break;
                case 'S':{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].DownKey, IsDown, WasDown);
                }break;
                case 'D':{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].RightKey, IsDown, WasDown);
                }break;
                case VK_SHIFT:{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].ShiftKey, IsDown, WasDown);
                }break;
                case VK_SPACE:{
                    ProcessGameButtonState(&GlobalGameInput->controllers[cid].SpaceKey, IsDown, WasDown);
                }break;
                }
                if (IsDown){
                    if (vkey == VK_ESCAPE){
                        GlobalGameInput->controllers[cid].CapturingMouse = !GlobalGameInput->controllers[cid].CapturingMouse;
                    }
                }
            }
            if (IsDown){
                if (AltKeyWasDown == true && vkey == VK_F4){
                    GlobalRunning = false;
                }
                if (AltKeyWasDown == true && vkey == VK_RETURN){
                    Win32ToggleFullscreen(GlobalScreen.Window);
                }
            }
        }break;
        default:{
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }break;
        }
    }
}

LRESULT CALLBACK
Win32WindowProcessing(
    HWND Window,
    UINT Message,
    WPARAM WParam,
    LPARAM LParam)
{

    switch (Message){
    case WM_CLOSE:{
        GlobalRunning = false;
    }break;
    case WM_DESTROY:{
        GlobalRunning = false;
    }break;
    case WM_QUIT:{
        GlobalRunning = false;
    }break;

    default:{
        return DefWindowProc(Window, Message, WParam, LParam);
    }break;
    }

    return 0;
}

static void
Win32RecreateWindow(
    win32_screen_buffer* Buffer,
    HINSTANCE Instance,
    const char* windowName,
    int windowWidth,
    int windowHeight)
{
    UnregisterClass(L"WindowClassName", Instance);
    DestroyWindow(Buffer->Window);

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.hInstance = Instance;
    wcex.lpfnWndProc = Win32WindowProcessing;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpszClassName = L"WindowClassName";
    RegisterClassEx(&wcex);

    Buffer->Window = CreateWindowEx(
        0,
        wcex.lpszClassName,
        L"CubeProject",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowWidth,
        windowHeight,
        0,
        0,
        Instance,
        0);
}


int WINAPI WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR ComandLine,
    int ComandShow)
{
	game_memory GameMemory = {};
    GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
    GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
    GameMemory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;

    for (int i = 0; i < 4; ++i){
        GlobalGameInput->controllers[i].CapturingMouse = true;
    }

    int windowWidth = 1366;
    int windowHeight = 768;
    Win32InitScreenBuffer(&GlobalScreen, windowWidth, windowHeight);
    Win32InitGameTime(GlobalTime);

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.hInstance = Instance;
    wcex.lpfnWndProc = Win32WindowProcessing;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpszClassName = L"WindowClassName";
    RegisterClassEx(&wcex);

    GlobalScreen.Window = CreateWindowEx(
        0,
        wcex.lpszClassName,
        L"CubeProject",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowWidth,
        windowHeight,
        0,
        0,
        Instance,
        0);

    InitOpenGL(&GlobalScreen, Instance, false, 0);
    glViewport(0, 0, GlobalScreen.Width, GlobalScreen.Height);

    Win32LoadTrueTypeFont("../data/Fonts/LiberationMono-Bold.ttf", 48);
    GlobalCharacterAtlas = Win32LoadTrueTypeFontToAtlas("../data/Fonts/LiberationMono-Bold.ttf");


    float screenCenterX = (float)GlobalScreen.Width / 2.0f;
    float screenCenterY = (float)GlobalScreen.Height / 2.0f;

    GlobalRunning = true;
    while (GlobalRunning == true){

        ProcessPendingMessagesToController(0);

        if (GlobalGameInput->controllers[0].CapturingMouse == true){
            POINT MouseP;
            GetCursorPos(&MouseP);
            GlobalGameInput->controllers[0].DeltaMouseX = (float)MouseP.x - screenCenterX;
            GlobalGameInput->controllers[0].DeltaMouseY = (float)MouseP.y - screenCenterY;
            SetCursorPos(screenCenterX, screenCenterY);
        }

        ProcessBeginOfFrameTime(GlobalTime);

        game_screen_buffer Buffer;
        Buffer.Memory = GlobalScreen.Memory;
        Buffer.Width = GlobalScreen.Width;
        Buffer.Height = GlobalScreen.Height;
        Buffer.currentWidth = GlobalScreen.currentWidth;
        Buffer.currentHeight = GlobalScreen.currentHeight;

		platform_dependent_context WtfContext;
		WtfContext.Atlas = GlobalCharacterAtlas;
		WtfContext.Characters = characters;
		WtfContext.Time = &GlobalTime;
		WtfContext.Camera = GlobalGameCamera;

        GameUpdateAndRender(&GameMemory, GlobalGameInput, &Buffer, &WtfContext);


        HDC swapDC = GetDC(GlobalScreen.Window);
        SwapBuffers(swapDC);
        ReleaseDC(GlobalScreen.Window, swapDC);

        ProcessEndOfFrameTime(GlobalTime);
    }

    delete[] characters;

    return 0;
}
