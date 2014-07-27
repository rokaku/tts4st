# Created by Charles Lu

#---------------------------------------------------
# makefile - makefile for win32
#---------------------------------------------------

TARGET_PLATFORM=WIN32

SRCPATH=../common

PLATFORM_CFLAGS = -c -nologo -W3 -G6 -GD -O1 -DNO_DEBUG -DWIN32 -DUNICODE -D_UNICODE -D_WIN32_DCOM -INCREMENTAL:NO
PLATFORM_LFLAGS = -machine:I386 -subsystem:windows
PLATFORM_LLIBS  = user32.lib ole32.lib Advapi32.lib delayimp.lib ibmeci.lib #spchwrap.lib winmm.lib  

PLATFORM_DLL_FLAGS = /dll /DELAYLOAD:ibmeci.dll /pdb:none

#INCPATH= $(J9JNI)
#INCPATH = -I"$(JRE_BIN)\include" -I"$(SRCPATH)"
INCPATH= -I$(J9PLATFORM)\include -I$(SRCPATH)

CPP=cl.exe
#CFLAGS= $(cflags) -D_MT -D_DLL -MD -D_WINSOCKAPI_ -DWIN32 -Ogityb1 -Gs -GF -Zm300 -Zi -I$(INCPATH)
CFLAGS= $(cflags) -D_MT -D_DLL -MD -Gs -GF -Zi $(INCPATH) $(PLATFORM_CFLAGS)

DLLNAME=tts.dll# declaration
LIBNAME=tts# declaration
MODULE=tts# declaration
MAPFILE=$(LIBNAME).map
OUTDIR=.\build

#.cpp.obj:
#	$(CPP) $(CFLAGS) $*.cpp

BUILDFILES = ttstrace.obj SAPI4.obj sapi5.obj eciSynthese.obj tts.obj tts.res
#stdafx.obj vtobj.obj ttsfnc.obj ecidlg.obj

$(DLLNAME): $(BUILDFILES) 
    @echo Linking $@
    link \
    $(VMLINK) $(ldebug) $(dlllflags) \
    $(PLATFORM_LFLAGS) -out:$(DLLNAME) -map:$(MAPFILE) \
    $(BUILDFILES) \
    $(PLATFORM_LLIBS) $(PLATFORM_DLL_FLAGS) \
    $(guilibsmt) 

tts.res:
    @echo Building $@ $(ECHOFILE)
    rc $(RCFLAGS) -r -fo $@ tts.rc
    
tts.obj: tts.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) tts.cpp

SAPI4.obj: SAPI4.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) SAPI4.cpp

SAPI5.obj: SAPI5.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) SAPI5.cpp
    
eciSynthese.obj: eciSynthese.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) eciSynthese.cpp

ttstrace.obj: ttstrace.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) ttstrace.cpp

ecidlg.obj: ecidlg.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) ecidlg.cpp

ttsfnc.obj: ttsfnc.cpp
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) ttsfnc.cpp

vtobj.obj: vtobj.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) vtobj.cpp

stdafx.obj: stdafx.cpp  
    @echo Building $@ $(ECHOFILE)
    $(CPP) $(CFLAGS) stdafx.cpp
   
all: $(DLLNAME)    
    
clean:
	-del *.obj
	-del *.lib
	-del *.dll
	-del *.map
	-del *.exp
	-del *.pdb	   
	-del *.res