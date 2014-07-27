//Created by Charles Lu 
#include <windows.h>
#include <initguid.h>
#include <stdio.h>
#include <speech.h>

#include "tts.h"
#include "sapi4.h"

//////From SAPI Samples
/*
void CloneAndTest(PITTSENUM pTTSEnum)
{
	TTSMODEINFO aTTSInfo[MAX_ENUMMODES];
	HRESULT     hRes = S_OK;
	DWORD       dwNumTimes,i;
	TCHAR       szStr[256],szMfg[40],szName[80];
	int j=0;
	int nskip = 3;
	while (hRes == S_OK)
	{
		// enumerate 2 items
		hRes = pTTSEnum->Next (2, aTTSInfo, &dwNumTimes);
		_stprintf (szStr, TEXT("  %d mode(s) found:\r\n"), (int) dwNumTimes);
		//MainOutput (szStr);
		for (i = 0; i < dwNumTimes; i++, j++)
		{
			S2ACVTEX(szName, aTTSInfo[j].szModeName, APPMAXSTRLEN(szName) + 1)
			S2ACVTEX(szMfg, aTTSInfo[j].szMfgName, APPMAXSTRLEN(szMfg) + 1)
			_stprintf( szStr, TEXT("    Mode %2d = %s, %s\r\n"), j+1, szMfg, szName );
			//MainOutput (szStr);
		}

		if(SUCCEEDED(hRes))
		{
		// skip nskip
		j += nskip;
		hRes = pTTSEnum->Skip( nskip );
	}
}*/

DWORD EnumModes( PTTSMODEINFO aTTSInfo, int maxnmodes )
{
    ttsTrace("SAPI4::EnumModes... %d", g_pITTSEnum);

	PITTSENUM   pClone1, pClone2;
	HRESULT     hRes;
	DWORD       dwTotalModes,dwNumTimes;
//	TCHAR       szStr[256];
    
	// create an enumerator object
	if(!g_pITTSEnum) return 0;
	
	ttsTrace("SAPI4::EnumModes...0");
	hRes = g_pITTSEnum->Clone (&pClone1);
	if( FAILED(hRes) )	return 0;

    ttsTrace("SAPI4::EnumModes...1");
	// enumerate up to maxnmodes modes and stuff into global aTTSInfo array...
	hRes = pClone1->Next (maxnmodes, aTTSInfo, &dwNumTimes);
//	_stprintf (szStr, TEXT("  %d mode(s) found:\r\n"), (int) dwNumTimes);
//	MainOutput (szStr);
	if( dwNumTimes == 0 ) return 0;

/*	for (i = 0; i < dwNumTimes; i++)
	{
		DisplayModeInfo( &aTTSInfo[i] );
	}*/

	dwTotalModes = dwNumTimes;	 // remember the total number of modes

	// reset
	if( FAILED((hRes = pClone1->Reset())) )
	{
//		MainOutput (TEXT("Enum->Reset() failed.\r\n"));
		pClone1->Release();
		return dwTotalModes;
	}

	// clone it and test the clone...
	ttsTrace("SAPI4::EnumModes...2");
	if( FAILED((hRes=pClone1->Clone (&pClone2))) )
	{
//		MainOutput (TEXT("Enum->Clone() failed.\r\n"));
		pClone2 = NULL;
	}
	else
	{
	 	//CloneAndTest(pClone2);
	}



	if( pClone2 ) //if( pClone2->Release() ) MainOutput (TEXT("pClone2->Release() failed.\r\n"));
	    pClone2->Release();

	// free the enumerator
	//if( pClone1->Release() ) MainOutput (TEXT("Enum->Release() failed.\r\n"));
	ttsTrace("SAPI4::EnumModes...3");
	pClone1->Release();

	return dwTotalModes;
}

int GetModes(char Array[][ARRAYMAZLENGTH]) 
{
    ttsTrace("SAPI4::GetModes...");
    
    int gnmodes = (int)EnumModes( gaTTSInfo, MAX_ENUMMODES );	 
    
	for(int i=0;i<gnmodes;i++) {
	    if(Array!= NULL) {
    	    int len=(lstrlenW(gaTTSInfo[i].szModeName)+1)<<1;
    	    WideCharToMultiByte(CP_ACP, 0, gaTTSInfo[i].szModeName, -1, Array[i], len, NULL, NULL);
    	    Array[i][len]=0;
	    }
	}
	
	return gnmodes;
}

bool SAPIInit(void)
{
    ttsTrace("SAPI4::SAPIInit...");
    
	// create engine data structure...
	gpEng = (PENGDATA)new EngData_t;
	memset( gpEng, 0, sizeof(EngData_t) );
		
    // Create an instance of the buffer notification object based on the
    // application-defined CBufNotify class.
    if ((g_pIBufNotifySink = new CBufNotify()) == NULL)
        return FALSE;	
        
        
	return TRUE;
}

bool SetVoice(int voice)
{
    ttsTrace("SAPI4::SetVoice...");
    
    PIAUDIOMULTIMEDIADEVICE	pIAMM = NULL;    
    HRESULT     hRes;    
    PITTSFIND pITTSFind;   // address of ITTSFind interface
    
    GetModes(NULL); //refill the gaTTSInfo structure
    

    hRes = CoCreateInstance(CLSID_MMAudioDest, NULL, CLSCTX_INPROC_SERVER, IID_IAudioMultiMediaDevice, (void**)&pIAMM);
    if (FAILED(hRes)) {
        return FALSE;
    }
    
#ifdef _DEBUG
    FILE *fp=_tfopen(L"c:\\z.log",L"w+");		
	_ftprintf(fp, _T("AAA"));
	fflush(fp);
	fclose(fp);
#endif
    
    hRes = CoCreateInstance(CLSID_TTSEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_ITTSFind, (void**) &pITTSFind);
    if (FAILED(hRes)) {    
                 
        return FALSE;  
    }      
	
    hRes = pITTSFind->Select(gaTTSInfo[voice].gModeID, &g_pITTSCentral, pIAMM);
    if (FAILED(hRes)) {        
        pIAMM->Release();
        pITTSFind->Release();
        return FALSE;
    }

    pITTSFind->Release();
    pIAMM->Release();            
    return TRUE;    
}

bool FindAndSelectEngine(void)
{
    ttsTrace("SAPI4::FindAndSelectEngine...");
    
    HRESULT hRes;          // result code
    PITTSFIND pITTSFind;   // address of ITTSFind interface   
    TTSMODEINFO  modewanted, modefound; 
    PIAUDIOMULTIMEDIADEVICE	pIAMM = NULL;
    
    // Initialize OLE.
//    if (FAILED(CoInitialize(NULL)))
//        return FALSE;

    // Create the multimedia audio-destination object and retrieve the
    // address of the object's IAudioMultiMediaDevice interface.
    ttsTrace("SAPI4::FindAndSelectEngine...1");
    hRes = CoCreateInstance(CLSID_MMAudioDest, NULL, CLSCTX_INPROC_SERVER, IID_IAudioMultiMediaDevice, (void**)&pIAMM);
    if (FAILED(hRes)) {
        ttsTrace("SAPI4::FindAndSelectEngine...1(%d)",hRes);
        return FALSE;
    }
    hRes = pIAMM->DeviceNumSet(WAVE_MAPPER);
    g_pIUnkAudio = pIAMM;
    g_pIUnkAudio->AddRef();
    
    ttsTrace("SAPI4::FindAndSelectEngine...2");
    hRes = CoCreateInstance (CLSID_TTSEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_ITTSEnum, (void**)&g_pITTSEnum);
	if (FAILED(hRes))
	{
	    pIAMM->Release();
	    return FALSE;
	}
    
    // Create the text-to-speech enumerator and retrieve the address of
    // the object's ITTSFind interface.
    ttsTrace("SAPI4::FindAndSelectEngine...3");
    //hRes = CoCreateInstance(CLSID_TTSEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_ITTSFind, (void**) &pITTSFind);
    hRes = g_pITTSEnum->QueryInterface(IID_ITTSFind, (void**) &pITTSFind);
    if (FAILED(hRes)) {
        g_pITTSEnum->Release();
        pIAMM->Release();        
        return FALSE;  
    }  
            
    // Set the device identifier for the multimedia wave-out device.
    ttsTrace("SAPI4::FindAndSelectEngine...4");    
    
    // Define the characteristics of the desired text-to-speech mode.
    ZeroMemory(&modewanted, sizeof(modewanted));
    
    //modewanted.language.LanguageID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
    //modewanted.wGender = GENDER_FEMALE;
    modewanted.wGender = GENDER_MALE;
    modewanted.wAge = TTSAGE_ADULT;
    //lstrcpy(modewanted.szStyle, (const unsigned short *)"Business");
    //_tcscpy(modewanted.szStyle, _T("Business"));
    
    // Find the engine and mode that most closely matches the desired characteristics.
    ttsTrace("SAPI4::FindAndSelectEngine...5");
    hRes = pITTSFind->Find(&modewanted, NULL, &modefound);
    if (FAILED(hRes)) {
        g_pITTSEnum->Release();        
        pITTSFind->Release();
        pIAMM->Release();
        return FALSE;
    }
    
    // Select the text-to-speech mode and create an engine object to
    // represent the mode. Also, retrieve the address of the engine's
    // ITTSCentral interface.
    ttsTrace("SAPI4::FindAndSelectEngine...6");
    hRes = pITTSFind->Select(modefound.gModeID, &g_pITTSCentral, (LPUNKNOWN) pIAMM);
    if (FAILED(hRes)) {
        g_pITTSEnum->Release();               
        pITTSFind->Release();
        pIAMM->Release(); 
        return FALSE;
    }
        
    ttsTrace("SAPI4::FindAndSelectEngine...7");
    hRes = g_pITTSCentral->QueryInterface (IID_ITTSDialogs, (void**)&g_pITTSDialogs);
          
    // Release the text-to-speech enumerator; it is no longer needed.
    ttsTrace("SAPI4::FindAndSelectEngine...8");
    pITTSFind->Release();
    pIAMM->Release();
    
    return TRUE;
}

bool Dialog(HWND hwnd, BOOL isAboutDlg) {
    ttsTrace("SAPI4::Dialog ...");
    
	if(g_pITTSDialogs) {
	    HRESULT hRes;
	    if (isAboutDlg) {
		    hRes = g_pITTSDialogs->AboutDlg( hwnd, NULL );
        }else{
		    hRes = g_pITTSDialogs->GeneralDlg( hwnd, NULL ); 
        }	    	
    }
    return TRUE;
}

bool SpeakTheText(LPCTSTR pszTextBuffer)
{
    ttsTrace("SAPI4::SpeakTheText...");
    
    HRESULT hRes;
    SDATA text;

    if(g_pITTSCentral)
    {
        text.dwSize = lstrlen(pszTextBuffer)*2+1;
        text.pData = (void*) pszTextBuffer;
      	/*APIALLOC(pOutStr, pszTextBuffer.GetLength());
    	APIA2SCVT(pOutStr, pszTextBuffer);      
    	
    	text.pData = pOutStr;
    	text.dwSize = APISIZEOFBUF(pOutStr);*/
    		
        // Put the text buffer into the engine's speaking queue, and give
        // the engine the address of the ITTSBufNotifySink interface for
        // buffer-related notifications.    
        /*hRes = g_pITTSCentral->TextData(CHARSET_TEXT, 0, text,
            g_pIBufNotifySink, IID_ITTSBufNotifySink);*/
            
        hRes = g_pITTSCentral->TextData(CHARSET_TEXT, 0, text,
    		NULL, IID_ITTSBufNotifySink);
            
        if (FAILED(hRes))
            return FALSE;
    }
    //APIFREE(pOutStr);    
    return TRUE;
}
//////// End of SAPI Example

void StopSynthesis() 
{
    ttsTrace("SAPI4::StopSynthesis...");
	if(g_pITTSCentral)
		g_pITTSCentral->AudioReset();
}

void UnloadSAPI4()
{	
    ttsTrace("SAPI4::UnloadSAPI4...");

	if(g_pITTSDialogs) {g_pITTSDialogs->Release(); g_pITTSDialogs=NULL;}
//	if(g_pITTSEnum) {g_pITTSEnum->Release(); g_pITTSEnum=NULL;}    //don't know why it'll cause exception??
	if(g_pITTSCentral) {g_pITTSCentral->Release(); g_pITTSCentral=NULL;}
	
	while (g_pIUnkAudio && g_pIUnkAudio->Release());
//	CoUninitialize ();		
}

/*************************************************************************
CBufNotify - Notification object.
*/
CBufNotify::CBufNotify (void)
{
// this space intentionally left blank
}

CBufNotify::~CBufNotify (void)
{
// this space intentionally left blank
}

STDMETHODIMP CBufNotify::QueryInterface (REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;

	/* always return our IUnknown for IID_IUnknown */
	if (IsEqualIID (riid, IID_IUnknown) || IsEqualIID(riid,IID_ITTSBufNotifySink))
	{
		*ppv = (LPVOID) this;
		return S_OK;
	}

	// otherwise, cant find
	return ResultFromScode (E_NOINTERFACE);
}

STDMETHODIMP_ (ULONG) CBufNotify::AddRef (void)
{
	// normally this increases a reference count, but this object
	// is going to be freed as soon as the app is freed, so it doesn't
	// matter
	return 1;
}

STDMETHODIMP_(ULONG) CBufNotify::Release (void)
{
	// normally this releases a reference count, but this object
	// is going to be freed when the application is freed so it doesnt
	// matter
	return 1;
}

void CBufNotify::NotifyOutput( const /*TCHAR*/char *str )
{
/*	CTTStstDlg *dlgtmp = (CTTStstDlg *)AfxGetApp()->m_pMainWnd;
	CTabITTSNotify *tabtmp = &(dlgtmp->m_Notify);

	tabtmp->Output(str);*/
}

STDMETHODIMP CBufNotify::BookMark (QWORD qTimeStamp, DWORD dwMarkNum)
{
	//ASSERT(gpEng);
	if( gpEng->Not.bBookMark )
	{
		//TCHAR  buf[256];
		//_stprintf( buf, TEXT("\r\nBookMark: qTimeStamp=%0I64x, dwMarkNum=%0lx\r\n"), (QWORD) qTimeStamp, (DWORD) dwMarkNum);
		char buf[256];
		sprintf( buf, "\r\nBookMark: qTimeStamp=%0I64x, dwMarkNum=%0lx\r\n", (QWORD) qTimeStamp, (DWORD) dwMarkNum);
		NotifyOutput( buf );
	}

   return NOERROR;
}

STDMETHODIMP CBufNotify::TextDataDone (QWORD qTimeStamp, DWORD dwFlags)
{
	//ASSERT(gpEng);
	if( gpEng->Not.bTextDataDone )
	{
		//TCHAR  buf[256];
		//_stprintf( buf, TEXT("\r\nTextDataDone: qTimeStamp=%0I64x, dwFlags=%0lx\r\n"), (QWORD) qTimeStamp, (DWORD) dwFlags);
		char buf[256];
		sprintf( buf, "\r\nTextDataDone: qTimeStamp=%0I64x, dwFlags=%0lx\r\n", (QWORD) qTimeStamp, (DWORD) dwFlags);
		NotifyOutput( buf );
	}

   return NOERROR;
}

STDMETHODIMP CBufNotify::TextDataStarted (QWORD qTimeStamp)
{
	//ASSERT(gpEng);
	if( gpEng->Not.bTextDataStarted )
	{
		//TCHAR  buf[256];
		//_stprintf( buf, TEXT("\r\nTextDataStarted: qTimeStamp=%0I64x\r\n"), (QWORD) qTimeStamp);
		char buf[256];
		sprintf( buf, "\r\nTextDataStarted: qTimeStamp=%0I64x\r\n", (QWORD) qTimeStamp);
		NotifyOutput( buf );
	}

   return NOERROR;
}

STDMETHODIMP CBufNotify::WordPosition (QWORD qTimeStamp, DWORD dwByteOffset)
{
	//ASSERT(gpEng);
	if( gpEng->Not.bWordPosition )
	{
		//TCHAR  buf[256];
		//_stprintf( buf, TEXT("\r\nWordPosition: qTimeStamp=%0I64x, dwByteOffset=%0lx\r\n"), (QWORD) qTimeStamp, (DWORD) dwByteOffset);
		char buf[256];
		sprintf( buf, "\r\nWordPosition: qTimeStamp=%0I64x, dwByteOffset=%0lx\r\n", (QWORD) qTimeStamp, (DWORD) dwByteOffset);
		NotifyOutput( buf );
	}

   return NOERROR;
}
