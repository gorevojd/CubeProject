@echo off

rem set "GLEW_DIR=..\Include\GLEW\"
rem set "GLM_DIR=..\Include\GLM"
rem set "SOIL_DIR=..\Include\SOIL\"
rem set "FREETYPE_DIR=..\Include\FREETYPE\"
rem set "FBX_SDK_DIR=..\Include\FBX_SDK\2015.1\"
rem set "ASSIMP_DIR=..\Include\ASSIMP\assimp-3.1.1\"

set CommonCompilerFlags=/EHsc /nologo /Zi /MTd
set CommonCompilerFlags=%CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\GLEW\include" %CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\GLM" %CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\SOIL\src" %CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\FREETYPE\include\freetype2" %CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\FBX_SDK\2015.1\include" %CommonCompilerFlags%
set CommonCompilerFlags=/I"..\Include\ASSIMP\assimp-3.1.1\include" %CommonCompilerFlags%
set CommonCompilerFlags=/DGLEW_STATIC %CommonCompilerFlags%
set CommonCompilerFlags=/D_UNICODE /DUNICODE %CommonCompilerFlags%

set CommonLinkerFlags=/incremental:no /opt:ref
set CommonLinkerFlags=/LIBPATH:"..\Include\SOIL\lib" %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:"..\Include\GLEW\lib\Release\Win32" %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:"..\Include\FREETYPE\lib" %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:"..\Include\FBX_SDK\2015.1\lib\vs2013\x86\debug" %CommonLinkerFlags%
set CommonLinkerFlags=/LIBPATH:"..\Include\ASSIMP\assimp-3.1.1\build\code\Debug" %CommonLinkerFlags%
set CommonLinkerFlags=kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib %CommonLinkerFlags%
set CommonLinkerFlags=assimpd.lib glew32s.lib wininet.lib %CommonLinkerFlags%
set CommonLinkerFlags=libfbxsdk-mt.lib SOIL.lib freetype.lib %CommonLinkerFlags%
set CommonLinkerFlags=/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRTD %CommonLinkerFlags%

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

IF NOT EXIST ..\BatBuild mkdir ..\BatBuild
pushd ..\BatBuild

cl %CommonCompilerFlags% ..\Code\win32_code.cpp /link %CommonLinkerFlags%

popd
