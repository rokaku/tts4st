#include <windows.h>
#include <eci.h>
#include "ttstrace.h"

#define ARRAYMAXSIZE 50     //my computer always has more than 30 SAPI4 TTS runtimes
#define ARRAYMAZLENGTH 70   //the max length of SAPI4 voice name is about 59 characters

bool FindAndSelectEngine(void);
bool FindAndSelectEngine5(void);
bool SpeakTheText(LPCTSTR pszTextBuffer);
bool SpeakTheText5(LPCTSTR pszTextBuffer);
bool ECISpeakTheText(const char* message);
//bool GetViaVoiceTTS(BYTE** pIBMTTS);
bool InitECIEngine();
void StopSynthesis();
void StopSynthesis5();
void StopECI();
bool SAPIInit(void);
void UnloadSAPI4();
void UnloadSAPI5();
void UnloadECI();
void MessageLoopInit();
void MessageLoopDestroy();
int GetLanguages(int displayedLanguage, char[][ARRAYMAZLENGTH]);
int GetModes(char[][ARRAYMAZLENGTH]);
bool Dialog(HWND, BOOL);
bool SetLanguageVoice(int, int, int);
bool SetVoice(int voice);

const int TTS_NOTFOUND= 0; 
const int TTS_SAPI5_STARTED= 0x01;  
const int TTS_SAPI5_STOPPED= 0x02;  
const int TTS_SAPI4_STARTED= 0x04; 
const int TTS_SAPI4_STOPPED= 0x08; 
const int TTS_IBM_STARTED= 0x10;    
const int TTS_IBM_STOPPED= 0x20;    

//ECIHand eciInstance = NULL_ECI_HAND;