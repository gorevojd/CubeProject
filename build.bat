@echo off

set "GLEW_DIR=..\Include\GLEW\"
set "GLM_DIR=..\Include\GLM"
set "SOIL_DIR=..\Include\SOIL\"
set "FREETYPE_DIR=..\Include\FREETYPE\"
set "FBX_SDK_DIR=..\Include\FBX_SDK\2015.1\"
set "ASSIMP_DIR=..\Include\ASSIMP\assimp-3.1.1\"

set CommonCompilerFlags=-EHsc -nologo -Z7
set CommonCompilerFlags=%CommonCompilerFlags%
set CommonCompilerFlags=/I%GLEW_DIR%include %CommonCompilerFlags%
set CommonCompilerFlags=/I%GLM_DIR% %CommonCompilerFlags%
set CommonCompilerFlags=/I%SOIL_DIR%src %CommonCompilerFlags%
set CommonCompilerFlags=/I%FREETYPE_DIR%include\freetype2 %CommonCompilerFlags%
set CommonCompilerFlags=/I%FBX_SDK_DIR%include %CommonCompilerFlags%
set CommonCompilerFlags=/I%ASSIMP_DIR%include %CommonCompilerFlags%
set CommonCompilerFlags=/DGLEW_STATIC %CommonCompilerFlags%
set CommonCompilerFlags=/D_UNICODE /DUNICODE %CommonCompilerFlags%

set CommonLinkerFlags=-incremental:no -opt:ref 
set CommonLinkerFlags=/LIBPATH:%SOIL_DIRL%lib %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:%GLEW_DIR%lib\Release\Win32 %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:%FREETYPE_DIR%lib %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:%FBX_SDK_DIR%lib\vs2013\x86\debug %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:%ASSIMP_DIR%build\code\Debug %CommonLinkerFlags%
set CommonLinkerFlags=kernel32.lib user32.lib gdi32.lib winmm.lib opengl32.lib %CommonLinkerFlags%
set CommonLinkerFlags=wininet.lib assimpd.lib opengl32.lib glew32s.lib %CommonLinkerFlags%
set CommonLinkerFlags=libfbxsdk-md.lib SOIL.lib freetype.lib %CommonLinkerFlags%

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

IF NOT EXIST ..\BatBuild mkdir ..\BatBuild
pushd ..\BatBuild

cl %CommonCompilerFlags% ..\Code\win32_code.cpp /link %CommonLinkerFlags%

popd
