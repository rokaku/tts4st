rem @echo off

setlocal

echo ---------------------------------------------------------------
echo Building Win32 natives
echo ---------------------------------------------------------------

set J9PLATFORM=e:\WEME\env\jsr-base\jre
set TTSRUNTIMES="c:\progra~1\viavoice tts"


set DEV_ROOT=%1
set MSVC=msvc
set SDK=sdk
set JRE_BIN=..\..\..\..\jre\bin
set J9PLATFORM=%J9PLATFORM%

echo.
echo INFO:  The following paths have been set:
echo         - Microsoft Visual C++ = %DEV_ROOT%\%MSVC%
echo         - Microsoft Platform SDK = %DEV_ROOT%\%SDK%
echo         - Project Root = %PROJECT_ROOT%
echo		 - J9PLATFORM = %J9PLATFORM%
echo		 - TTSRUNTIMES = %TTSRUNTIMES%


set INCLUDE=%J9PLATFORM%\bin\include;%TTSRUNTIMES%;%INCLUDE%;C:\Program Files\Microsoft Speech SDK\Include;C:\Program Files\Microsoft Speech SDK 5.1\Include
set LIB=%LIB%;%J9PLATFORM%\include\LIB;%TTSRUNTIMES%;C:\Program Files\Microsoft Speech SDK\Lib;C:\PROGRAM FILES\MICROSOFT SPEECH SDK 5.1\LIB\I386
     
rem set INCLUDE=%DEV_ROOT%\%MSVC%\ATL\Include;%DEV_ROOT%\%MSVC%\Include;%DEV_ROOT%\%SDK%\Include;%INCLUDE%;%J9PLATFORM%\bin\include
rem set LIB=%DEV_ROOT%\%MSVC%\Lib;%DEV_ROOT%\%SDK%\Lib;%LIB%;%J9PLATFORM%\include\LIB
set PATH=%DEV_ROOT%\%MSVC%\Bin;%DEV_ROOT%\%MSVC%\common\Tools;%DEV_ROOT%\%MSVC%\common\MSDev98\Bin;%DEV_ROOT%\%SDK%\Bin;%PATH%

nmake clean

nmake %1 %2 %3

rem copy steci.dll ..\..\os\win32\x86
echo.
echo End of build.bat
:ERROR
endlocal