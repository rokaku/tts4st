//Created by charleslu@tw.ibm.com
//Code from http://msdn.microsoft.com/en-us/library/ms724256(VS.85).aspx
#include "tts.h"
#include "string.h"
#include "TTSMessageHandler.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

ECIHand eciInstance = NULL_ECI_HAND;
const int NUM_LANGUAGES = 20;
const int NUM_VOICES = 8;

//typedef ECIHand (__stdcall *eciNew)  (void);

const struct
{
    char *name;
    ECILanguageDialect param;
    char *paramString;
    char *testString;
}
LANGUAGES[NUM_LANGUAGES] =
{
    { "US English",           eciGeneralAmericanEnglish, "eciGeneralAmericanEnglish", "This is a test."           },
    { "UK English",           eciBritishEnglish,         "eciBritishEnglish",         "This is a test."           },
    { "Spanish",              eciCastilianSpanish,       "eciCastilianSpanish",       "Esto es una demostraciÛn." },
    { "Mexican Spanish",      eciMexicanSpanish,         "eciMexicanSpanish",         "Esto es una prueba."       },
    { "French",               eciStandardFrench,         "eciStandardFrench",         "Ceci est un test."         },
    { "Canadian French",      eciCanadianFrench,         "eciCanadianFrench",         "Ceci est un test."         },
    { "German",               eciStandardGerman,         "eciStandardGerman",         "Jetzt wird getestet."      },
    { "Italian",              eciStandardItalian,        "eciStandardItalian",        "Questo Ë un test."         },
    { "Simplified Chinese",   eciMandarinChinese,        "eciMandarinChinese",        "’‚ «“ª∏ˆ≤‚ ‘°£"            },
    { "Traditional Chinese",  eciTaiwaneseMandarin,      "eciTaiwaneseMandarin",      "≥o¨O§@≠”¥˙∏’°C"            },
    { "Brazilian Portuguese", eciBrazilianPortuguese,    "eciBrazilianPortuguese",    "Isto È um teste."          },
    { "Japanese",             eciStandardJapanese,       "eciStandardJapanese",       "Ç±ÇÍÇÕÉeÉXÉgÇ≈Ç∑ÅB"        },
    { "Finnish",              eciStandardFinnish,        "eciStandardFinnish",        "T‰m‰ on testi."            },
    { "Korean",               eciStandardKorean,         "eciStandardKorean",         "¿Âƒ° Ω√«Ë¡ﬂ ¿‘¥œ¥Ÿ."       },
    { "Cantonese",            eciStandardCantonese,      "eciStandardCantonese",      ""                          }, // TODO: Add text
    { "Hong Kong Cantonese",  eciHongKongCantonese,      "eciHongKongCantonese",      ""                          }, // TODO: Add text
    { "Dutch",                eciStandardDutch,          "eciStandardDutch",          "Zulks zit te toets."       }, // auto-generated // TODO: Replace with phrase-splicing sample
    { "Norwegian",            eciStandardNorwegian,      "eciStandardNorwegian",      "Du ¯nsker Â overf¯re fire millioner tohundre tusen kroner fra din sparekonto til din personkonto er det korrekt."}, // phrase-splicing sample
    { "Swedish",              eciStandardSwedish,        "eciStandardSwedish",        "Den andra februari nitton hundra Âttiosex sattes Âttahundra nittioÂtta kronor och tjugofyra ˆre in pÂ kontot."   }, // phrase-splicing sample
    { "Danish",               eciStandardDanish,         "eciStandardDanish",         "Den nuvÊrende saldo er ti kroner og fem ¯re ¯nsker du at godkende."                                              }, // phrase-splicing sample
};

static int AvailableLangs[NUM_LANGUAGES+1]={0};	//keep the lang index (index started from 0).
static int SelectedLanguage=-1;

bool GetViaVoiceTTS(BYTE** pIBMTTS)
{
    ttsTrace("ECI::GetViaVoiceTTS...");
    
    HKEY hKey;
    DWORD size;
/*    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cValues;              // number of values for key 
    DWORD    cbName;                   // size of name string 
    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; */
    
//    DWORD num_keys, length_longest_keyname, length_keyname;	    
	BYTE *p;    
	PPERF_DATA_BLOCK PerfData = NULL;

    
    ttsTrace("ECI::GetViaVoiceTTS...1");        
    
    //Only works for TTS v6.x
    long lRc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,         
                            TEXT("SOFTWARE\\IBM\\ViaVoice TTS"),  
                            0,   // reserved
                            KEY_QUERY_VALUE, 
                            &hKey    // handle to open key
                            );    
/*    DWORD lRc = RegOpenKeyEx(HKEY_LOCAL_MACHINE,         
                            TEXT("SOFTWARE\\IBM\\ViaVoice\\Outloud Runtime"),  
                            0,   // reserved
                            KEY_READ, 
                            &hKey    // handle to open key
                            );    
                            
    if (lRc == ERROR_SUCCESS) 
    {
    	lRc = RegQueryInfoKey (
                  hKey,
                  NULL,
                  NULL,
                  NULL,
                  &cSubKeys,
                  NULL,
                  NULL,
                  &cValues,
                  NULL,
                  NULL,
                  NULL,
                  NULL);

        ttsTrace("cSubKeys=(%d), cValues=%d", cSubKeys, cValues);
        if (cSubKeys) {
            for (DWORD i=0; i<cSubKeys; i++) 
            { 
                cbName = MAX_KEY_LENGTH;
                lRc = RegEnumKeyEx(hKey, i,
                         achKey, 
                         &cbName, 
                         NULL, 
                         NULL, 
                         NULL, 
                         NULL); 
                if (lRc == ERROR_SUCCESS) 
                {
                    //ttsTrace("(%d) %s", i, achKey);
                    _tcscpy(achValue,TEXT("SOFTWARE\\IBM\\ViaVoice\\Outloud Runtime\\"));
                    _tcscat(achValue,achKey);
                    ttsTrace("New Reg=%s", achValue);
                    
                    DWORD lRc2 = RegOpenKeyEx(HKEY_LOCAL_MACHINE,         
                            achValue,  
                            0,   // reserved
                            KEY_READ, 
                            &hKey    // handle to open key
                            );    
                                    
                } else
                    ttsTrace("RegEnumKeyEx err=%d", lRc);
            }         	
        }
        //return false;   //for debug
    } else
     	return false;*/
     	
    if (lRc != ERROR_SUCCESS) return false;

    //This '\0' is important!!
    ttsTrace("ECI::GetViaVoiceTTS...2");
    if (RegQueryValueEx(hKey, TEXT("Path\0"), NULL, NULL, (LPBYTE)PerfData, &size) != ERROR_SUCCESS)  return false;
        
    p = (BYTE*)malloc(size);
    ttsTrace("ECI::GetViaVoiceTTS...3");
    lRc = RegQueryValueEx(  hKey,
                            TEXT("Path\0"),
                            NULL,
                            NULL,
                            (LPBYTE)p,
                            &size);
    
    ttsTrace("ECI::GetViaVoiceTTS...4");
    RegCloseKey(hKey);
    
    if (lRc != ERROR_SUCCESS) 
        return false;
        
    *pIBMTTS = p;
//    ttsTrace("ECI::GetViaVoiceTTS...5 (%d)", size);
    return true;
}

int GetLanguages(int displayedLanguage, char Array[][ARRAYMAZLENGTH])
{
    ttsTrace("ECI::GetLanguages: (%d)", (Array!=NULL));
    int languageParam;
    int rc;
    int numAvailableLanguages = 0;
    ECILanguageDialect *languages;
    int i, j;
    int numDisplayedLanguages = 0;

    // Get previous language
    languageParam = eciGetParam(eciInstance, eciLanguageDialect);

    // Get number of available languages
    rc = eciGetAvailableLanguages(0, &numAvailableLanguages); // only works if numAvailableLanguages is 0
    if (rc != ECI_NOERROR)
    {
        // Error
        //error.Format("Error: eciGetAvailableLanguages() returned %d", rc);
        //MessageBox(error, APP_NAME, MB_ICONERROR);
        return false;
    }
    if (numAvailableLanguages < 1)
    {
        // Error
        //MessageBox("Error: No ECILanguageDialects are registered", APP_NAME, MB_ICONERROR);
        return false;
    }
    languages = (ECILanguageDialect *)malloc(numAvailableLanguages * sizeof(ECILanguageDialect));

    // Get languages
    rc = eciGetAvailableLanguages(languages, &numAvailableLanguages);
    if (rc != ECI_NOERROR)
    {
        // Error
        //error.Format("Error: eciGetAvailableLanguages() returned %d", rc);
        //MessageBox(error, APP_NAME, MB_ICONERROR);
        free(languages);
        return false;
    }

    // Display first language if previous language not found
    //displayedLanguage = 0;

    // Fill combobox
    int index=0, count=0;
    for (i = 0; i < numAvailableLanguages; i++)
    {
        // Find index of this language
        for (j = 0; j < NUM_LANGUAGES; j++)
        {
            if (LANGUAGES[j].param == languages[i])
            {
                //SendDlgItemMessage(IDC_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)LANGUAGES[j].name);
                //SendDlgItemMessage(IDC_LANGUAGE, CB_SETITEMDATA, numDisplayedLanguages, j);
                
                //Set language parameter, otherwise return all available ECI languages
                if(displayedLanguage>0) {
                    if(count++ == displayedLanguage)
                        eciSetParam(eciInstance, eciLanguageDialect, LANGUAGES[j].param);
                }
                else {
                	AvailableLangs[index] = j;
                    //if(Array!=NULL) _tcscpy(Array[index], LANGUAGES[j].name);
                    if(Array!=NULL) strcpy(Array[index], LANGUAGES[j].name);
                    index++;
                }
                
                if (LANGUAGES[j].param == languageParam)
                {
                    // Found previous language, so display it
                    displayedLanguage = numDisplayedLanguages;
                }
                
                numDisplayedLanguages++;
                
                break;
            }
        }
    }

    // Free array from call to GetAvailableLanguages()
    free(languages);

    if (numDisplayedLanguages < 1)
    {
        // Error
        //MessageBox("Error: No known ECILanguageDialects are registered", APP_NAME, MB_ICONERROR);
        return 0;
    }
    
    // Select displayedLanguage
    //SendDlgItemMessage(IDC_LANGUAGE, CB_SETCURSEL, *displayedLanguage, 0);

    // Success   
    return numAvailableLanguages;
}

bool SetLanguageVoice(int lang, int voice, int sampleRate=0) 
{
    ttsTrace("ECI::SetLanguage...");
    
    int baseVoice = 0;
    
    switch (voice) {        
        case 1:
            baseVoice = 2;  //adult female
            break;
        case 2:
            baseVoice = 3;  //child
            break;
        case 3:
            baseVoice = 7;  //elderly male
            break;
        case 4:
            baseVoice = 8;  //elderly female
            break;
        default:
            baseVoice = 1;  //adult male
    }
    
    
    eciCopyVoice(eciInstance, baseVoice, 0);
    
    eciSetParam(eciInstance, eciSampleRate, sampleRate); // use 8k
    
    if(SelectedLanguage==-1) GetLanguages(0, NULL);	//initialize AvailableLangs array
    SelectedLanguage=AvailableLangs[lang];
    eciSetParam(eciInstance, eciLanguageDialect, LANGUAGES[SelectedLanguage].param);
    int lan=eciGetParam(eciInstance, eciLanguageDialect);
#ifdef _DEBUG    
	FILE *fp=fopen("C:\\b.log","w+");
	if(fp!=NULL) {
		int actualsize=fwrite((void*)LANGUAGES[SelectedLanguage].paramString, sizeof(char), 20,fp);		
		fwrite((void*)"\n", sizeof(char), 2,fp);		
		if(lan==eciTaiwaneseMandarin) fwrite((void*)"GOOD\n", sizeof(char), 6,fp);		
		if(lan==eciGeneralAmericanEnglish) fwrite((void*)"BAD\n", sizeof(char), 6,fp);	
		
		fflush(fp);
		fclose(fp);
	}
#endif
    
    return true;    
}

void StopECI()
{
    ttsTrace("ECI::Stop...");
    
	if (eciInstance == NULL_ECI_HAND) 	return;
	//int state = (jbool == JNI_TRUE ? true : false);
	///eciPause(eciInstance, state);	
	///if (state == false)
	int state = eciStop(eciInstance);
	
	if (state == false) {
		char msg[100]={0};
		eciErrorMessage(eciInstance, msg);
		printf(msg);
	}    	    
}    

bool InitECIEngine()
{
    ttsTrace("ECI::InitEngine...");

    TCHAR workingDirectory[MAX_PATH]={0};
    LPBYTE IBMTTSDirectory;
    
    if(!GetViaVoiceTTS(&IBMTTSDirectory)) 
    {
        free(IBMTTSDirectory);
        return JNI_FALSE;    
    }
    
    ttsTrace("ECI::InitEngine TTS Dir= (%S)", IBMTTSDirectory);    
    GetCurrentDirectory(MAX_PATH, workingDirectory);
    ttsTrace("ECI::InitEngine Work Dir= (%S)", workingDirectory);    
    SetCurrentDirectory((LPTSTR)IBMTTSDirectory);
    
    ttsTrace("ECI::InitEngine...eciInstance = %d", eciInstance == NULL_ECI_HAND);    
    //FUNC   func   =   (FUNC) GetProcAddress(eciNew());   
	if (eciInstance == NULL_ECI_HAND) eciInstance  = eciNew();
    
    ttsTrace("ECI::InitEngine...setCurrentDir");
	SetCurrentDirectory(workingDirectory);		
	free(IBMTTSDirectory);
	
	if (eciInstance == NULL_ECI_HAND) 
		return false;
	else
		return true;             
}

void UnloadECI()
{
    ttsTrace("ECI::Unload...");
	// Free ECI
	if (eciInstance == NULL_ECI_HAND) 	return;
	eciDelete(eciInstance);
	eciInstance = NULL_ECI_HAND;
	//if (g_hinstLib != NULL) FreeLibrary(g_hinstLib);    
}

bool ECISpeakTheText(const char* message)
{
    ttsTrace("ECI::SpeakTheText...");
    
    if (eciInstance == NULL_ECI_HAND) return false;
	    
        //prepare for UNICODE        
        /*switch (SelectedLanguage) {
        	case 8:
        	case 9:
				setlocale(LC_TYPE, "cht");
     			int len = strlen((char*)message)+1;
     			unsigned short *ut= (unsigned short*) malloc(len*sizeof(short));
			     if(ut) {
			         mbstowcs(ut, (char*)message, len);
			         eciAddText(eciInstance, ut);
			         free(ut);    
			     }

        	case 11:
        	case 13:
        	case 15:
        	break;
        	
        	case else:
        		eciAddText(eciInstance , message);
			break;
		}	*/
    	    
///     setlocale(LC_CTYPE, "C");        
    	eciAddText(eciInstance , message);
    	eciSynthesize(eciInstance);   	
    	//eciSynchronize(eciInstance);
    		
	return true;	
}