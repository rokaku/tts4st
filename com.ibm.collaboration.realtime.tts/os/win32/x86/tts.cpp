//Created by charleslu@tw.ibm.com

#include <jni.h>

#include "TTSMessageHandler.h"
#include "TTS.h"

// The handle to the dll
HINSTANCE g_hInstance = NULL;

// The handle to the thread that runs the filesystem registry message loop
HANDLE g_hMessageLoopThread = NULL;

// The handle to the filesystem registry message window
HWND g_hWnd = NULL;

//HINSTANCE g_hinstLib = NULL;

int nSelectedRuntime;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) 
    {		
		case WM_CLOSE: 
			DestroyWindow(g_hWnd);
			return 0;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

        default: 
            return DefWindowProc(hWnd, uMsg, wParam, lParam); 
    }
}

bool CreateHiddenWindow()
{
	WNDCLASSEX wcex;
	char szClassName[]="charleslu\0";
	char szWindowTitle[]="STECI\0";
	
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= (LPTSTR) szClassName;
	wcex.hIconSm		= NULL;
	if(!::RegisterClassEx(&wcex))
		return false;
	
	g_hWnd = CreateWindow((LPTSTR)szClassName, (LPTSTR)szWindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, g_hInstance, NULL);    
    if(g_hWnd==NULL) return false;
    		
	ShowWindow(g_hWnd,SW_HIDE);
	return true;
}

DWORD WINAPI MessageLoopProc(LPVOID lpParameter)
{
	MSG msg;
	BOOL bRet;
	
	CreateHiddenWindow();	
	while( (bRet = GetMessage( &msg, g_hWnd, 0, 0 )) != 0)
	{ 
	    if (bRet == -1)
	    {
	        // handle the error and possibly exit
	        return 0;
	    }
	    else
	    {
	        TranslateMessage(&msg); 
	        DispatchMessage(&msg); 
	    }
	}
	return 0;
}

void MessageLoopInit()
{
	// spawn the message loop thread
	g_hMessageLoopThread = CreateThread(
		NULL,
		0,
		MessageLoopProc,
		NULL,
		0,
		NULL);
	
	if (g_hMessageLoopThread == NULL) {
		// fail
	}
}

void MessageLoopDestroy()
{
	if (g_hWnd != NULL) {
		// destroy the window (will exit the message loop)
		SendMessage(g_hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		g_hWnd = NULL;
	}
}

JNIEXPORT void JNICALL JNI_Onload(JavaVM *vm, void *reserved)
{
    ttsTrace("tts::Onload...");
	//I don't know why the following does not work...?
    //CoInitialize(NULL);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    ttsTrace("tts::OnUnload...");
	//MessageLoopDestroy();
	CoUninitialize (); 
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSInit
(JNIEnv *, jclass)
{
    ttsTrace("tts::TTSInit: (%d)", nSelectedRuntime);
    if(nSelectedRuntime == TTS_SAPI4_STARTED)
    {
    	SAPIInit();    
        if(FindAndSelectEngine()) 
    	{
    		//SpeakTheText(TEXT("testing SAPI4"));
    		return JNI_TRUE;
        }
        else
            return JNI_FALSE;
    }
    else if(nSelectedRuntime == TTS_SAPI5_STARTED)
    {
        if(FindAndSelectEngine5())         
    	{
    		//SpeakTheText5(TEXT("testing SAPI5"));
    		return JNI_TRUE;
        }
        else
            return JNI_FALSE;
    }
    else
    {
        if(InitECIEngine())
            return JNI_TRUE;
        else
            return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSMessageHandler_TTSSpeak
(JNIEnv *env, jclass jc, jstring jstr)
{        
    
	//const char* message = env->GetStringUTFChars( jstr, 0);    
    const jchar* message = env->GetStringChars( jstr, 0);   //convert to Unicode        
    jsize nUniLength = env->GetStringLength(jstr);
    ttsTrace("tts::TTSSpeak: (%S)", message);

#ifndef LINUX 
	if(nSelectedRuntime == TTS_SAPI4_STARTED)    
        SpeakTheText(message);	    	    
    else if(nSelectedRuntime == TTS_SAPI5_STARTED)
    	SpeakTheText5(message);	    	    
	else 
#endif LINUX	
	{
//	    if (eciInstance  == NULL_ECI_HAND) return JNI_FALSE;
	    
	    //convert to native code
        char tmp[255];
        ::memset((void*)tmp, 0, sizeof(tmp));
            
        //unicode to native code        
        DWORD actualLength = WideCharToMultiByte(CP_ACP, 0, message, nUniLength, tmp, sizeof(tmp),NULL,NULL);   
        tmp[actualLength]=0;
	    
    	ECISpeakTheText(tmp);   
    }    
	env->ReleaseStringChars(jstr, message); 	
	
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSStop
(JNIEnv *, jclass cls, jboolean jbool)
{
    ttsTrace("tts::TTSStop: (%d)", nSelectedRuntime);
    if(nSelectedRuntime == TTS_SAPI4_STARTED) 
        StopSynthesis();
    else if(nSelectedRuntime == TTS_SAPI5_STARTED)
        StopSynthesis5();
    else 
        StopECI();
	return JNI_TRUE;
}

//Set TTS runtime, multiple choosens are acceptable by first matching
JNIEXPORT jint JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_setRuntime
(JNIEnv *, jclass cls, jint nTTS)
{
    ttsTrace("tts::setRuntime: (%d)", nTTS);
    if(nTTS & TTS_SAPI5_STARTED)    
        nSelectedRuntime = TTS_SAPI5_STARTED;    
    else if(nTTS & TTS_SAPI4_STARTED)    
        nSelectedRuntime = TTS_SAPI4_STARTED;    
    else if(nTTS & TTS_IBM_STARTED)    
        nSelectedRuntime = TTS_IBM_STARTED;    

    return nSelectedRuntime;
}
 
JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSUninit
(JNIEnv *, jclass)
{
    ttsTrace("tts::TTSUninit...");
    //Stop first, then unload.
    Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSStop(NULL, NULL, false);
    
    if(nSelectedRuntime == TTS_SAPI5_STARTED) 
        UnloadSAPI5();        
    else if (nSelectedRuntime == TTS_SAPI4_STARTED)
    	UnloadSAPI4();        
    else 
        UnloadECI();
            	
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_SAPI4Dialog
(JNIEnv *, jclass, HWND hWnd)
{
	/*if(gpITTSDialogs)
		HRESULT hRes = gpITTSDialogs->GeneralDlg( hWnd, NULL );*/
	
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSDialog
(JNIEnv *, jclass, jboolean isAboutDlg)
{
    ttsTrace("tts::TTSDialog: (%d)", nSelectedRuntime);
    
    if(nSelectedRuntime == TTS_SAPI4_STARTED) {
        Dialog(g_hWnd, isAboutDlg);
    }
    else if(nSelectedRuntime == TTS_SAPI5_STARTED)
    {
        //StopSynthesis5();
    }
    //ECI was done on Java side
    		
	return JNI_TRUE;
}


// nSelection : to choose which runtimes will be enabled
//
//TTS runtimes detection routines
JNIEXPORT jint JNICALL Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSDetect
(JNIEnv *, jclass, jint nSelection)
{           
    ttsTrace("nSelection=%d", nSelection);
    
    CoInitialize(NULL);
    //CoInitializeEx(NULL, COINIT_MULTITHREADED); //allowed to call COM from different threads
    //CoInitializeEx(NULL, NULL);
    
    int results=0;
    //if (Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSInit(NULL, NULL) == JNI_TRUE) 
    if(nSelection & TTS_IBM_STARTED) 
    {
        if(InitECIEngine())
        {
            UnloadECI();
            results |= TTS_IBM_STARTED;
            //Java_com_ibm_collaboration_realtime_tts_TTSPlugin_TTSUninit(NULL, NULL);
        }
    }

    MessageLoopInit();    //Put here to always create message pump until closing the plugin
    //nSelectedRuntime = TTS_SAPI4_STARTED;
    
    if(nSelection & TTS_SAPI4_STARTED) 
    {
        if(FindAndSelectEngine()) 
        {
            UnloadSAPI4();
            results |= TTS_SAPI4_STARTED;
        }
    }

    if(nSelection & TTS_SAPI5_STARTED) 
    {        
        if(FindAndSelectEngine5())   
        {
            UnloadSAPI5();
            results |= TTS_SAPI5_STARTED;
        }
    }        
    ttsTrace("tts::TTSDetect: (%d)", results);
    //CoUninitialize();
    
    return results;
}

JNIEXPORT jobjectArray JNICALL Java_com_ibm_collaboration_realtime_tts_preferences_PreferenceDialog_GetLanguages
(JNIEnv *env, jclass jc, jint selection)
{
    ttsTrace("tts::GetLanguages: (%d)", selection);
    
    //WCHAR currCat[128]=L"Hello";
    char currCat[ARRAYMAXSIZE][ARRAYMAZLENGTH]={0};
    jobjectArray stringArrayData = env->NewObjectArray(ARRAYMAXSIZE, env->FindClass("java/lang/String"), NULL);
    int count;
    
    if(selection==2)    //SAPI4
        count = GetModes(currCat);
    else if(selection==3) //ECI
        count = GetLanguages(0, currCat);
    
   	for(int i=0; i<count; i++)
	{
        //env->SetObjectArrayElement(stringArrayData, i, env->NewString((jchar*)currCat, wcslen(currCat)));
        env->SetObjectArrayElement(stringArrayData, i, env->NewStringUTF(currCat[i]));
    }
    
    return stringArrayData;
    //return 0;
}

JNIEXPORT jboolean JNICALL Java_com_ibm_collaboration_realtime_tts_preferences_PreferenceDialog_SetLanguageVoice
(JNIEnv *env, jclass jc, jint lang, jint voice, jint sampleRate)
{   
    ttsTrace("tts::SetLanguageVoice: (lang:%d, voice:%d, sampleRate:%d)", lang, voice, sampleRate); 
    
    if(voice == -1 || sampleRate == -1) //SAPI4
        return SetVoice(lang);  
    else    //ECI
        return SetLanguageVoice(lang, voice, sampleRate)? JNI_TRUE:JNI_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Main Entry point for that DLL
BOOL WINAPI DllMain( 
  HINSTANCE hInstDLL,      // handle to DLL module
  DWORD dwReason,       // reason for calling function
  LPVOID lpvReserved    // reserved
    )
{
	switch( dwReason )
	{
	case DLL_PROCESS_ATTACH:
    	if (g_hInstance == NULL) g_hInstance = hInstDLL;
	    break;
	case DLL_PROCESS_DETACH:
		break;
	
	default:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	
    }    
    return TRUE;
}
