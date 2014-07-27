//#include <windows.h>
#include "sapi5.h" 
#include "tts.h"

bool FindAndSelectEngine5(void)
{
    ttsTrace("SAPI5::FindAndSelectEngine...");
    // Initialize OLE.
//    if (FAILED(CoInitialize(NULL)))
//        return false;
        
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void **)&pVoice);

    if( !SUCCEEDED( hr ) )
        return false;
    else
        return true;
}


bool SpeakTheText5(LPCTSTR pszTextBuffer)
{
    ttsTrace("SAPI5::SpeakTheText...");
    
	HRESULT             hr = S_OK;
    //WCHAR               *szWTextString = NULL;

	// only get the string if we're not paused
	if(!pVoice) return false;
	{
        // Find the length of the string in the buffer
        /*GETTEXTLENGTHEX gtlx;
        gtlx.codepage = 1200;
        gtlx.flags = GTL_DEFAULT;
        LONG lTextLen = SendDlgItemMessage( m_hWnd, IDE_EDITBOX, EM_GETTEXTLENGTHEX, (WPARAM) &gtlx, 0 );
        szWTextString = new WCHAR[ lTextLen + 1 ];*/

		/*GETTEXTEX	GetText;
		
		GetText.cb			  = (lTextLen + 1) * sizeof( WCHAR );
		GetText.codepage	  = 1200;
		GetText.flags		  = GT_DEFAULT;
		GetText.lpDefaultChar = NULL;
		GetText.lpUsedDefChar = NULL;*/
		
		// Get the string in a unicode buffer
		//SendDlgItemMessage( m_hWnd, IDE_EDITBOX, EM_GETTEXTEX, (WPARAM)&GetText, (LPARAM)szWTextString );

		// do we speak or interpret the XML
		hr = pVoice->Speak(pszTextBuffer, SPF_ASYNC, NULL);
		//hr = m_cpVoice->Speak( szWTextString, SPF_ASYNC | ((IsDlgButtonChecked( m_hWnd, IDC_SPEAKXML )) ? SPF_IS_XML : SPF_IS_NOT_XML), 0 );

        /*delete[] szWTextString;

		if( FAILED( hr ) )
		{
			TTSAppStatusMessage( m_hWnd, _T("Speak error\r\n") );
		} */
	}
	//m_bPause = FALSE;
	
	// Set state to run
	hr = pVoice->Resume();            

	if( FAILED( hr ) )
	{		
	    return false;
	}
	return true;
}

void StopSynthesis5()
{
    ttsTrace("SAPI5::StopSynthesis...");
    
    if(!pVoice) return;
    
    HRESULT hr = pVoice->Speak( NULL, SPF_PURGEBEFORESPEAK, 0 );

    if( FAILED( hr ) )
    {

    }
}

void UnloadSAPI5()
{
    ttsTrace("SAPI5::UnloadSAPI...");
    if(!pVoice) return;
    
	pVoice->Release();
	pVoice = NULL;
}