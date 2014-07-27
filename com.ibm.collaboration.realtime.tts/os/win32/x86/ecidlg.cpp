// voicetunerdlg.cpp : implementation file
//

#include "stdafx.h"
//#include "voicetuner.h"
#include "ecidlg.h"
//#include "saveasdlg.h"
//#include "codedlg.h"

#ifdef EMBEDDED
#include <ealmain.h>
#endif // EMBEDDED

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Version number
#define VERSION                 1

// Frequency to check synthesis state
#define TIMER_FREQUENCY         100 // in milliseconds

// Suffixes to append to voice names
#define SUFFIX_PRESET           " (preset voice)"
#define SUFFIX_USER_DEFINED     " (based on %s at %d kHz)"

// Registry location of TTS path
#define SUBKEY_TTS              "SOFTWARE\\IBM\\ViaVoice TTS"
#define REGKEY_TTS_PATH         "Path"

// PropStorage path for application data
#ifdef EMBEDDED
#define PATH_VOICE_TUNER        "Voice Tuner for Embedded"
#else
#define PATH_VOICE_TUNER        "Voice Tuner"
#endif // EMBEDDED

// PropStorage keys for version, language, voice, sample rate, and file name
#define KEY_VERSION             "Version"
#define KEY_LANGUAGE            "Language"
#define KEY_PRESET_VOICE_NUMBER "Preset Voice Number"
#define KEY_VOICE_NAME          "Voice Name"
#define KEY_SAMPLE_RATE         "Sample Rate" // also used for each user-defined voice
#define KEY_TO_FILE             "To File"
#define KEY_FILE_NAME           "File Name"

// PropStorage keys for user-defined voices
#define KEY_BASE_VOICE          "Base Voice"
#define KEY_REAL_WORLD_UNITS    "Real World Units"
#define KEY_GENDER              "Gender"
#define KEY_PITCH_BASELINE      "Pitch Baseline"
#define KEY_SPEED               "Speed"
#define KEY_VOLUME              "Volume"
#define KEY_HEAD_SIZE           "Head Size"
#define KEY_PITCH_FLUCTUATION   "Pitch Fluctuation"
#define KEY_ROUGHNESS           "Roughness"
#define KEY_BREATHINESS         "Breathiness"

// The one and only CVoicetunerApp object
extern CVoicetunerApp theApp;

// Language data
// Update these consts together
// New languages do not have to be placed at the end
const int NUM_LANGUAGES = 20;
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
    { "Spanish",              eciCastilianSpanish,       "eciCastilianSpanish",       "Esto es una demostración." },
    { "Mexican Spanish",      eciMexicanSpanish,         "eciMexicanSpanish",         "Esto es una prueba."       },
    { "French",               eciStandardFrench,         "eciStandardFrench",         "Ceci est un test."         },
    { "Canadian French",      eciCanadianFrench,         "eciCanadianFrench",         "Ceci est un test."         },
    { "German",               eciStandardGerman,         "eciStandardGerman",         "Jetzt wird getestet."      },
    { "Italian",              eciStandardItalian,        "eciStandardItalian",        "Questo è un test."         },
    { "Simplified Chinese",   eciMandarinChinese,        "eciMandarinChinese",        "ÕâÊÇÒ»¸ö²âÊÔ¡£"            },
    { "Traditional Chinese",  eciTaiwaneseMandarin,      "eciTaiwaneseMandarin",      "³o¬O¤@­Ó´ú¸Õ¡C"            },
    { "Brazilian Portuguese", eciBrazilianPortuguese,    "eciBrazilianPortuguese",    "Isto é um teste."          },
    { "Japanese",             eciStandardJapanese,       "eciStandardJapanese",       "‚±‚ê‚ÍƒeƒXƒg‚Å‚·B"        },
    { "Finnish",              eciStandardFinnish,        "eciStandardFinnish",        "Tämä on testi."            },
    { "Korean",               eciStandardKorean,         "eciStandardKorean",         "ÀåÄ¡ ½ÃÇèÁß ÀÔ´Ï´Ù."       },
    { "Cantonese",            eciStandardCantonese,      "eciStandardCantonese",      ""                          }, // TODO: Add text
    { "Hong Kong Cantonese",  eciHongKongCantonese,      "eciHongKongCantonese",      ""                          }, // TODO: Add text
    { "Dutch",                eciStandardDutch,          "eciStandardDutch",          "Zulks zit te toets."       }, // auto-generated // TODO: Replace with phrase-splicing sample
    { "Norwegian",            eciStandardNorwegian,      "eciStandardNorwegian",      "Du ønsker å overføre fire millioner tohundre tusen kroner fra din sparekonto til din personkonto er det korrekt."}, // phrase-splicing sample
    { "Swedish",              eciStandardSwedish,        "eciStandardSwedish",        "Den andra februari nitton hundra åttiosex sattes åttahundra nittioåtta kronor och tjugofyra öre in på kontot."   }, // phrase-splicing sample
    { "Danish",               eciStandardDanish,         "eciStandardDanish",         "Den nuværende saldo er ti kroner og fem øre ønsker du at godkende."                                              }, // phrase-splicing sample
};

// Number of preset voices
extern const int NUM_VOICES = 8;

// Map from eciSampleRate parameters to kHz
// Update these consts together
extern const int NUM_SAMPLE_RATES = 3;
const int SAMPLE_RATES[NUM_SAMPLE_RATES] = 
{
    8,  // 8.000 kHz
    11, // 11.025 kHz
    22, // 22.050 kHz
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoicetunerDlg dialog

CVoicetunerDlg::CVoicetunerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVoicetunerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoicetunerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

bool CVoicetunerDlg::VoiceNameExists(const char *voiceName)
{
    int i;

    for (i = 0; i < m_voices.GetSize(); i++)
    {
        if (strcmp(m_voices[i], voiceName) == 0)
        {
            // Found
            return true;
        }
    }

    // Not found
    return false;
}

void CVoicetunerDlg::GetCode(CString& code)
{
    CString line;
    CString codeString;
    char nameBuffer[ECI_VOICE_NAME_LENGTH + 1];
    CEdit *testEdit;
    int lineCount, lineLength;
    char *testLine;
    int i;

    // Initialize code
    code.Empty();

    // Beginning
#ifdef EMBEDDED
    code += "#include <ealmain.h>\r\n";
#endif // EMBEDDED
    code += "#include <eci.h>\r\n";
    code += "\r\n";
    code += "int main()\r\n";
    code += "{\r\n";
    code += "\tECIHand eciInstance;\r\n";
    code += "\r\n";

    // Initialize ECI
    line.Format("\t// Initialize ECI for %s\r\n", LANGUAGES[m_languageIndex].name);
    code += line;
#ifdef EMBEDDED
    code += "\tealOpen();\r\n";
#endif // EMBEDDED
    line.Format("\teciInstance = eciNewEx(%s);\r\n", LANGUAGES[m_languageIndex].paramString);
    code += line;
    code += "\r\n";

    // Turn on annotations
    code += "\t// Turn on annotations\r\n";
    code += "\teciSetParam(eciInstance, eciInputType, 1);\r\n";
    code += "\r\n";

    // Set output file name
    if (m_toFile)
    {
        code += "\t// Set output file name\r\n";
        ConvertToCodeString(m_fileName, codeString);
        line.Format("\teciSetOutputFilename(eciInstance, \"%s\");\r\n", (LPCTSTR)codeString);
        code += line;
        code += "\r\n";
    }

    // Activate base voice
    if (m_sampleRate != 1 || m_baseVoice != 1)
    {
        eciGetVoiceName(m_eciInstance, m_baseVoice, nameBuffer);
        line.Format("\t// Activate preset voice %s at %d kHz\r\n", nameBuffer, SAMPLE_RATES[m_sampleRate]);
        code += line;
    }
    if (m_sampleRate != 1)
    {
        line.Format("\teciSetParam(eciInstance, eciSampleRate, %d);\r\n", m_sampleRate);
        code += line;
    }
    if (m_baseVoice != 1)
    {
        line.Format("\teciCopyVoice(eciInstance, %d, 0);\r\n", m_baseVoice);
        code += line;
    }
    if (m_sampleRate != 1 || m_baseVoice != 1)
    {
        code += "\r\n";
    }

    // Set voice parameters
    if (m_displayedVoice >= m_numPresetVoices || m_modified)
    {
        code += "\t// Set voice parameters\r\n";
        if (m_realWorldUnits)
        {
            code += "\teciSetParam(eciInstance, eciRealWorldUnits, 1);\r\n";
        }
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciGender, %d);\r\n", m_gender);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciPitchBaseline, %d);\r\n", m_pitchBaseline);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciSpeed, %d);\r\n", m_speed);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciVolume, %d);\r\n", m_volume);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciHeadSize, %d);\r\n", m_headSize);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciPitchFluctuation, %d);\r\n", m_pitchFluctuation);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciRoughness, %d);\r\n", m_roughness);
        code += line;
        line.Format("\teciSetVoiceParam(eciInstance, 0, eciBreathiness, %d);\r\n", m_breathiness);
        code += line;
        code += "\r\n";
    }

    // Synthesize text
    code += "\t// Synthesize text\r\n";
    testEdit = (CEdit *)GetDlgItem(IDC_TEST);
    lineCount = testEdit->GetLineCount();
    for (i = 0; i < lineCount; i++)
    {
        lineLength = testEdit->LineLength(testEdit->LineIndex(i));
        testLine = (char *)malloc(max((lineLength + 1) * sizeof(char), sizeof(int)));
        testEdit->GetLine(i, testLine, lineLength);
        testLine[lineLength] = '\0';
        ConvertToCodeString(testLine, codeString);
        free(testLine);
        line.Format("\teciAddText(eciInstance, \"%s\");\r\n", (LPCTSTR)codeString);
        code += line;
    }
    code += "\teciSynthesize(eciInstance);\r\n";
    code += "\teciSynchronize(eciInstance);\r\n";
    code += "\r\n";

    // Free ECI
    code += "\t// Free ECI\r\n";
    code += "\teciDelete(eciInstance);\r\n";
#ifdef EMBEDDED
    code += "\tealClose();\r\n";
#endif // EMBEDDED

    // End
    code += "\r\n";
    code += "\treturn 0;\r\n";
    code += "}\r\n";
}

BOOL CVoicetunerDlg::PreTranslateMessage(MSG* pMsg) 
{
    // Must override so that <Esc> key does not close window

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        // <Esc> key, so ignore
        return TRUE;
    }
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CVoicetunerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoicetunerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

bool CVoicetunerDlg::Initialize()
{
    int rc;
    CString error;
    int previousLanguageIndex;

    // Acquire timer
    m_timer = SetTimer(0, TIMER_FREQUENCY, NULL);
    if (!m_timer)
    {
        // Error
        MessageBox("Error: SetTimer() returned 0", APP_NAME, MB_ICONERROR);
        return false;
    }

    // Get PropStorage object
    m_propStorage = (IPropStorage *)GetViaVoicePropStorage(VIAVOICE_VERSION_IPROPSTORAGE);
    if (!m_propStorage)
    {
        // Error
        MessageBox("Error: GetViaVoicePropStorage() returned 0", APP_NAME, MB_ICONERROR);
        return false;
    }
    rc = m_propStorage->Initialize();
    if (rc != PROPSTORAGE_RC_OK)
    {
        // Error
        error.Format("Error: IPropStorage::Initialize() returned %d", rc);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        m_propStorage->Release();
        return false;
    }

    // Get index of previous language
    GetPreviousLanguageIndex(&previousLanguageIndex);

#ifdef EMBEDDED

    // Initialize EAL
    rc = ealOpen();
    if (rc != EAL_RC_OK)
    {
        error.Format("Error: ealOpen() returned %d", rc);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        m_propStorage->Release();
    }

    // Initialize engine
    InitializeEngine(previousLanguageIndex);

#else

    CString ttsDirectory;
    char workingDirectory[_MAX_PATH + 1];

    // Try initializing engine from working directory
    try
    {
        InitializeEngine(previousLanguageIndex);
    }
    catch (...)
    {
        // Error delay-loading ibmeci.dll from working directory,
        // so try initializing engine from location specified in registry
        if (!GetTTSDirectory(&ttsDirectory)) // get location of ibmeci.dll
        {
            m_propStorage->Release();
            return false;
        }
        GetCurrentDirectory(_MAX_PATH + 1, workingDirectory); // backup working directory
        if (!SetCurrentDirectory(ttsDirectory)) // set working directory to location of ibmeci.dll
        {
            error.Format("Error: Directory '%s' does not exist\n(see registry key 'HKEY_LOCAL_MACHINE\\%s\\%s')", (const char *)ttsDirectory, SUBKEY_TTS, REGKEY_TTS_PATH);
            MessageBox(error, APP_NAME, MB_ICONERROR);
            m_propStorage->Release();
            return false;
        }
        try
        {
            InitializeEngine(previousLanguageIndex);
        }
        catch (...)
        {
            // Error delay-loading ibmeci.dll from location specified in registry
            error.Format("Error: Unable to load 'ibmeci.dll' from '%s' (see registry key 'HKEY_LOCAL_MACHINE\\%s\\%s')", (const char *)ttsDirectory, SUBKEY_TTS, REGKEY_TTS_PATH);
            MessageBox(error, APP_NAME, MB_ICONERROR);
            m_propStorage->Release();
            return false;
        }
        SetCurrentDirectory(workingDirectory); // restore working directory
    }

#endif // EMBEDDED

    // Make sure engine was initialized successfully
    if (m_eciInstance == NULL_ECI_HAND)
    {
        // Error
        MessageBox("Error: eciNew() returned NULL_ECI_HAND", APP_NAME, MB_ICONERROR);
        m_propStorage->Release();
#ifdef EMBEDDED
        ealClose();
#endif // EMBEDDED
        return false;
    }

    // Turn on annotations
    eciSetParam(m_eciInstance, eciInputType, 1);

    // Not synthesizing yet
    m_synthesizing = false;

    // Success
    return true;
}

void CVoicetunerDlg::InitializeEngine(int previousLanguageIndex)
{
    m_eciInstance = NULL_ECI_HAND;

    if (previousLanguageIndex >= 0)
    {
        // Found previous language index, so specify language param
        m_eciInstance = eciNewEx(LANGUAGES[previousLanguageIndex].param);
    }

    if (m_eciInstance == NULL_ECI_HAND)
    {
        // Error in eciNewEx, or no previous language index, so use default language
        m_eciInstance = eciNew();
    }
}

bool CVoicetunerDlg::GetTTSDirectory(CString *ttsDirectory)
{
    HKEY hKey;
    DWORD type;
    LPBYTE value;
    DWORD size;
    CString error;

    // Open registry key
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, SUBKEY_TTS, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    {
        // Error
        error.Format("Error opening registry key 'HKEY_LOCAL_MACHINE\\%s'", SUBKEY_TTS);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        return false;
    }

    // Get value size
    if (RegQueryValueEx(hKey, REGKEY_TTS_PATH, NULL, &type, NULL, &size) != ERROR_SUCCESS)
    {
        // Error
        error.Format("Error querying registry key 'HKEY_LOCAL_MACHINE\\%s\\%s'", SUBKEY_TTS, REGKEY_TTS_PATH);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        RegCloseKey(hKey);
        return false;
    }

    // Make sure value is a string
    if (type != REG_SZ)
    {
        // Error
        error.Format("Error: Registry key 'HKEY_LOCAL_MACHINE\\%s\\%s' does not have a string value", SUBKEY_TTS, REGKEY_TTS_PATH);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        RegCloseKey(hKey);
        return false;
    }

    // Get value
    value = (LPBYTE)malloc(size);
    if (RegQueryValueEx(hKey, REGKEY_TTS_PATH, NULL, &type, value, &size) != ERROR_SUCCESS)
    {
        // Error
        error.Format("Error querying registry key 'HKEY_LOCAL_MACHINE\\%s\\%s'", SUBKEY_TTS, REGKEY_TTS_PATH);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        RegCloseKey(hKey);
        return false;
    }
    *ttsDirectory = value;
    free(value);

    // Close registry key
    RegCloseKey(hKey);

    // Success
    return true;
}

void CVoicetunerDlg::GetPreviousLanguageIndex(int *previousLanguageIndex)
{
    CVString previousLanguage;
    int rc;
    int i;

    // Get previous language
    m_propStorage->SetPath(PATH_VOICE_TUNER); // "Voice Tuner"
    rc = m_propStorage->Get(KEY_LANGUAGE, &previousLanguage);
    if (rc != PROPSTORAGE_RC_OK)
    {
        // Previous language not found
        *previousLanguageIndex = -1;
        return;
    }

    // Find index
    for (i = 0; i < NUM_LANGUAGES; i++)
    {
        if (strcmp(LANGUAGES[i].paramString, previousLanguage) == 0)
        {
            *previousLanguageIndex = i;
            return;
        }
    }

    // Previous language is unsupported
    *previousLanguageIndex = -1;
}

void CVoicetunerDlg::ActOnToFile(bool toFile)
{
    // Set m_toFile
    m_toFile = toFile;

    // Enable file name controls only if m_toFile is true
    GetDlgItem(IDC_FILENAME_STATIC)->EnableWindow(m_toFile);
    GetDlgItem(IDC_FILENAME_EDIT)->EnableWindow(m_toFile);
    GetDlgItem(IDC_BROWSE)->EnableWindow(m_toFile);

    // Update Synthesize/Stop button
    UpdateSynthesizeOrStop();
}

bool CVoicetunerDlg::FillLanguage(int *displayedLanguage)
{
    int languageParam;
    int rc;
    int numAvailableLanguages = 0;
    ECILanguageDialect *languages;
    int i, j;
    int numDisplayedLanguages = 0;
    CString error;

    // Get previous language
    languageParam = eciGetParam(m_eciInstance, eciLanguageDialect);

    // Get number of available languages
    rc = eciGetAvailableLanguages(0, &numAvailableLanguages); // only works if numAvailableLanguages is 0
    if (rc != ECI_NOERROR)
    {
        // Error
        error.Format("Error: eciGetAvailableLanguages() returned %d", rc);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        return false;
    }
    if (numAvailableLanguages < 1)
    {
        // Error
        MessageBox("Error: No ECILanguageDialects are registered", APP_NAME, MB_ICONERROR);
        return false;
    }
    languages = (ECILanguageDialect *)malloc(numAvailableLanguages * sizeof(ECILanguageDialect));

    // Get languages
    rc = eciGetAvailableLanguages(languages, &numAvailableLanguages);
    if (rc != ECI_NOERROR)
    {
        // Error
        error.Format("Error: eciGetAvailableLanguages() returned %d", rc);
        MessageBox(error, APP_NAME, MB_ICONERROR);
        free(languages);
        return false;
    }

    // Display first language if previous language not found
    *displayedLanguage = 0;

    // Fill combobox
    for (i = 0; i < numAvailableLanguages; i++)
    {
        // Find index of this language
        for (j = 0; j < NUM_LANGUAGES; j++)
        {
            if (LANGUAGES[j].param == languages[i])
            {
                SendDlgItemMessage(IDC_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)LANGUAGES[j].name);
                SendDlgItemMessage(IDC_LANGUAGE, CB_SETITEMDATA, numDisplayedLanguages, j);
                if (LANGUAGES[j].param == languageParam)
                {
                    // Found previous language, so display it
                    *displayedLanguage = numDisplayedLanguages;
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
        MessageBox("Error: No known ECILanguageDialects are registered", APP_NAME, MB_ICONERROR);
        return false;
    }

    // Select displayedLanguage
    SendDlgItemMessage(IDC_LANGUAGE, CB_SETCURSEL, *displayedLanguage, 0);

    // Success
    return true;
}

void CVoicetunerDlg::ActOnLanguageSelection(int displayedLanguage)
{
    CString path;
    int rc;
    int displayedVoice = -1;
    int presetVoiceNumber;
    int i;
    CVString voiceName;

    // Set m_displayedLanguage and m_languageIndex
    m_displayedLanguage = displayedLanguage;
    m_languageIndex = SendDlgItemMessage(IDC_LANGUAGE, CB_GETITEMDATA, m_displayedLanguage, 0);;

    // Call ECI to set language
    eciSetParam(m_eciInstance, eciLanguageDialect, LANGUAGES[m_languageIndex].param);

    // Fill Voice combobox
    FillVoice();

    // Default to 11 kHz
    m_sampleRate = 1;

    // Select previous voice
    path = PATH_VOICE_TUNER "/";
    path += LANGUAGES[m_languageIndex].paramString;
    m_propStorage->SetPath((LPCTSTR)path); // "Voice Tuner/<language>"
    rc = m_propStorage->Get(KEY_PRESET_VOICE_NUMBER, &presetVoiceNumber);
    if (rc == PROPSTORAGE_RC_OK)
    {
        // Previous voice found, so select it
        if (presetVoiceNumber > 0)
        {
            // Preset voice
            for (i = 0; i < m_numPresetVoices; i++)
            {
                if (SendDlgItemMessage(IDC_VOICE, CB_GETITEMDATA, i, 0) == presetVoiceNumber)
                {
                    displayedVoice = i;
                    SendDlgItemMessage(IDC_VOICE, CB_SETCURSEL, displayedVoice, 0);
#ifdef EMBEDDED
                    m_sampleRate = 1; // only supports 11 kHz
#else
                    m_propStorage->Get(KEY_SAMPLE_RATE, &m_sampleRate);
#endif // EMBEDDED
                    break;
                }
            }
        }
        else
        {
            // User-defined voice
            rc = m_propStorage->Get(KEY_VOICE_NAME, &voiceName);
            if (rc == PROPSTORAGE_RC_OK)
            {
                // Voice name found
                displayedVoice = SelectUserDefinedVoice(voiceName);
            }
        }
    }

    if (displayedVoice == -1)
    {
        // Previous voice not found, so select first voice
        displayedVoice = 0;
        SendDlgItemMessage(IDC_VOICE, CB_SETCURSEL, displayedVoice, 0);
        m_sampleRate = 1; // default to 11 kHz
    }

    // Load selected voice
    ActOnVoiceSelection(displayedVoice);

    // Fill Test edit box
    SetDlgItemText(IDC_TEST, LANGUAGES[m_languageIndex].testString);
}

void CVoicetunerDlg::SavePreviousVoice()
{
    CString path;

    // Save as previous voice for the current language
    path = PATH_VOICE_TUNER "/";
    path += LANGUAGES[m_languageIndex].paramString;
    m_propStorage->SetPath((LPCTSTR)path); // "Voice Tuner/<language>"
    m_propStorage->Set(KEY_VOICE_NAME, m_voiceName);
    if (m_displayedVoice < m_numPresetVoices)
    {
        // Preset voice
        m_propStorage->Set(KEY_PRESET_VOICE_NUMBER, m_baseVoice);
#ifndef EMBEDDED
        m_propStorage->Set(KEY_SAMPLE_RATE, m_sampleRate);
#endif // EMBEDDED
    }
    else
    {
        // User-defined voice
        m_propStorage->Set(KEY_PRESET_VOICE_NUMBER, 0);
    }
}

void CVoicetunerDlg::FillVoice()
{
    int i;
    char nameBuffer[ECI_VOICE_NAME_LENGTH + 1];
    CString voice, suffix;
    CString languagePath, voicePath;
    int baseVoice, sampleRate;

    // Remove all voices
    SendDlgItemMessage(IDC_VOICE, CB_RESETCONTENT, 0, 0);

    // Add preset voices
    m_numPresetVoices = 0;
    for (i = 1; i <= NUM_VOICES; i++)
    {
        if (eciCopyVoice(m_eciInstance, i, 0))
        {
            eciGetVoiceName(m_eciInstance, i, nameBuffer);
            voice.Format("%s%s", nameBuffer, SUFFIX_PRESET);
            SendDlgItemMessage(IDC_VOICE, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)voice);
            SendDlgItemMessage(IDC_VOICE, CB_SETITEMDATA, m_numPresetVoices, i);
            m_numPresetVoices++;
        }
    }

    // Add user-defined voices
    languagePath.Format("%s/%s", PATH_VOICE_TUNER, LANGUAGES[m_languageIndex].paramString);
    m_propStorage->SetPath((LPCTSTR)languagePath); // "Voice Tuner/<language>"
    m_propStorage->GetDirs(&m_voices);
    for (i = 0; i < m_voices.GetSize(); i++)
    {
        // Get base voice name and sample rate
        voicePath.Format("%s/%s", (LPCTSTR)languagePath, (const char *)m_voices[i]);
        m_propStorage->SetPath((LPCTSTR)voicePath); // "Voice Tuner/<language>/<voice>"
        m_propStorage->Get(KEY_BASE_VOICE, &baseVoice);
        eciGetVoiceName(m_eciInstance, baseVoice, nameBuffer);
        m_propStorage->Get(KEY_SAMPLE_RATE, &sampleRate);

        // Add "<voice> (based on <baseVoice> at <sampleRate> kHz)"
        suffix.Format(SUFFIX_USER_DEFINED, nameBuffer, SAMPLE_RATES[sampleRate]);
        voice.Format("%s%s", (const char *)m_voices[i], (LPCTSTR)suffix);
        SendDlgItemMessage(IDC_VOICE, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)voice);
    }
}

int CVoicetunerDlg::SelectUserDefinedVoice(const char *voiceName)
{
    int displayedVoice = 0; // display first voice if previous voice not found
    int i;

    // Find index of this voice
    for (i = 0; i < m_voices.GetSize(); i++)
    {
        if (strcmp(m_voices[i], voiceName) == 0)
        {
            // Found voice, add number of preset voices to get actual index
            displayedVoice = m_numPresetVoices + i;
            break;
        }
    }

    // Select voice
    SendDlgItemMessage(IDC_VOICE, CB_SETCURSEL, displayedVoice, 0);

    // Return index of displayed voice
    return displayedVoice;
}

void CVoicetunerDlg::ActOnVoiceSelection(int displayedVoice)
{
    char nameBuffer[ECI_VOICE_NAME_LENGTH + 1];
    CString path;

    // Set m_displayedVoice
    m_displayedVoice = displayedVoice;

    // Load voice
    if (m_displayedVoice < m_numPresetVoices)
    {
        // Preset voice, so get base voice
        m_baseVoice = SendDlgItemMessage(IDC_VOICE, CB_GETITEMDATA, m_displayedVoice, 0);

        // Preset voice, so set sample rate and then copy voice
        eciSetParam(m_eciInstance, eciSampleRate, m_sampleRate);
        eciCopyVoice(m_eciInstance, m_baseVoice, 0);

        // Call ECI to get voice name
        eciGetVoiceName(m_eciInstance, m_baseVoice, nameBuffer);
        m_voiceName = nameBuffer;

        // Don't use real world units
        m_realWorldUnits = 0;
        eciSetParam(m_eciInstance, eciRealWorldUnits, m_realWorldUnits);

        // Query ECI for voice parameters
        GetVoiceParameters();

#ifndef EMBEDDED
        // Enable sample rate
        GetDlgItem(IDC_SAMPLERATE_8)->EnableWindow(TRUE);
        GetDlgItem(IDC_SAMPLERATE_11)->EnableWindow(TRUE);
        GetDlgItem(IDC_SAMPLERATE_22)->EnableWindow(TRUE);
#endif // EMBEDDED

        // Disable delete
        GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
        if (GetFocus() == NULL)
        {
            // Focus was on Delete button, but now disabled, so move focus
            GetDlgItem(IDC_CODE)->SetFocus();
        }
    }
    else
    {
        // User-defined voice, so get voice name from array
        m_voiceName = m_voices[m_displayedVoice - m_numPresetVoices];

        // Set m_propStorage path to "Voice Tuner/<language>/<voice>"
        path = PATH_VOICE_TUNER "/";
        path += LANGUAGES[m_languageIndex].paramString;
        path += "/";
        path += m_voiceName;
        m_propStorage->SetPath((LPCTSTR)path);

        // Retrieve base voice, sample rate, and real world units
        m_propStorage->Get(KEY_BASE_VOICE, &m_baseVoice);
        m_propStorage->Get(KEY_SAMPLE_RATE, &m_sampleRate);
        m_propStorage->Get(KEY_REAL_WORLD_UNITS, &m_realWorldUnits);

        // Set real world units
        eciSetParam(m_eciInstance, eciRealWorldUnits, m_realWorldUnits);

        // Retrieve voice parameters
        m_propStorage->Get(KEY_GENDER, &m_gender);
        m_propStorage->Get(KEY_PITCH_BASELINE, &m_pitchBaseline);
        m_propStorage->Get(KEY_SPEED, &m_speed);
        m_propStorage->Get(KEY_VOLUME, &m_volume);
        m_propStorage->Get(KEY_HEAD_SIZE, &m_headSize);
        m_propStorage->Get(KEY_PITCH_FLUCTUATION, &m_pitchFluctuation);
        m_propStorage->Get(KEY_ROUGHNESS, &m_roughness);
        m_propStorage->Get(KEY_BREATHINESS, &m_breathiness);

#ifndef EMBEDDED
        // Disable sample rate
        GetDlgItem(IDC_SAMPLERATE_8)->EnableWindow(FALSE);
        GetDlgItem(IDC_SAMPLERATE_11)->EnableWindow(FALSE);
        GetDlgItem(IDC_SAMPLERATE_22)->EnableWindow(FALSE);
#endif // EMBEDDED

        // Enable delete
        GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
    }

    // Display sample rate, real world units, and voice parameters
    DisplaySampleRate();
    CheckDlgButton(IDC_REALWORLDUNITS, m_realWorldUnits);
    DisplayVoiceParameters();

    // Parameters just loaded, so not modified yet
    SetModified(false);
}

void CVoicetunerDlg::SetParameters()
{
    eciSetVoiceParam(m_eciInstance, 0, eciGender, m_gender);
    eciSetVoiceParam(m_eciInstance, 0, eciPitchBaseline, m_pitchBaseline);
    eciSetVoiceParam(m_eciInstance, 0, eciSpeed, m_speed);
    eciSetVoiceParam(m_eciInstance, 0, eciVolume, m_volume);
    eciSetVoiceParam(m_eciInstance, 0, eciHeadSize, m_headSize);
    eciSetVoiceParam(m_eciInstance, 0, eciPitchFluctuation, m_pitchFluctuation);
    eciSetVoiceParam(m_eciInstance, 0, eciRoughness, m_roughness);
    eciSetVoiceParam(m_eciInstance, 0, eciBreathiness, m_breathiness);
}

void CVoicetunerDlg::GetVoiceParameters()
{
    m_gender = eciGetVoiceParam(m_eciInstance, 0, eciGender);
    m_pitchBaseline = eciGetVoiceParam(m_eciInstance, 0, eciPitchBaseline);
    m_speed = eciGetVoiceParam(m_eciInstance, 0, eciSpeed);
    m_volume = eciGetVoiceParam(m_eciInstance, 0, eciVolume);
    m_headSize = eciGetVoiceParam(m_eciInstance, 0, eciHeadSize);
    m_pitchFluctuation = eciGetVoiceParam(m_eciInstance, 0, eciPitchFluctuation);
    m_roughness = eciGetVoiceParam(m_eciInstance, 0, eciRoughness);
    m_breathiness = eciGetVoiceParam(m_eciInstance, 0, eciBreathiness);
}

void CVoicetunerDlg::DisplaySampleRate()
{
    // Deselect all radio buttons
    CheckDlgButton(IDC_SAMPLERATE_8, 0);
    CheckDlgButton(IDC_SAMPLERATE_11, 0);
    CheckDlgButton(IDC_SAMPLERATE_22, 0);

    // Select appropriate radio button
    switch (m_sampleRate)
    {
    case 0:
        CheckDlgButton(IDC_SAMPLERATE_8, 1);
        break;
    case 1:
        CheckDlgButton(IDC_SAMPLERATE_11, 1);
        break;
    case 2:
        CheckDlgButton(IDC_SAMPLERATE_22, 1);
        break;
    }
}

void CVoicetunerDlg::DisplayVoiceParameters()
{
    // Gender
    if (m_gender == 0)
    {
        // Male
        CheckDlgButton(IDC_GENDER_MALE, 1);
        CheckDlgButton(IDC_GENDER_FEMALE, 0);
    }
    else // m_gender == 1
    {
        // Female
        CheckDlgButton(IDC_GENDER_MALE, 0);
        CheckDlgButton(IDC_GENDER_FEMALE, 1);
    }

    // Pitch Baseline
    if (m_realWorldUnits)
    {
        ((CSliderCtrl *)GetDlgItem(IDC_PITCHBASELINE_SLIDER))->SetRange(40, 422);
        ((CEdit *)GetDlgItem(IDC_PITCHBASELINE_EDIT))->SetLimitText(3);
    }
    else
    {
        ((CSliderCtrl *)GetDlgItem(IDC_PITCHBASELINE_SLIDER))->SetRange(0, 100);
        ((CEdit *)GetDlgItem(IDC_PITCHBASELINE_EDIT))->SetLimitText(3);
    }
    ((CSliderCtrl *)GetDlgItem(IDC_PITCHBASELINE_SLIDER))->SetPos(m_pitchBaseline);
    SetDlgItemInt(IDC_PITCHBASELINE_EDIT, m_pitchBaseline);

    // Speed
    if (m_realWorldUnits)
    {
        ((CSliderCtrl *)GetDlgItem(IDC_SPEED_SLIDER))->SetRange(70, 1297);
        ((CEdit *)GetDlgItem(IDC_SPEED_EDIT))->SetLimitText(4);
    }
    else
    {
        ((CSliderCtrl *)GetDlgItem(IDC_SPEED_SLIDER))->SetRange(0, 250);
        ((CEdit *)GetDlgItem(IDC_SPEED_EDIT))->SetLimitText(3);
    }
    ((CSliderCtrl *)GetDlgItem(IDC_SPEED_SLIDER))->SetPos(m_speed);
    SetDlgItemInt(IDC_SPEED_EDIT, m_speed);

    // Volume
    if (m_realWorldUnits)
    {
        ((CSliderCtrl *)GetDlgItem(IDC_VOLUME_SLIDER))->SetRange(1, 65535);
        ((CEdit *)GetDlgItem(IDC_VOLUME_EDIT))->SetLimitText(5);
    }
    else
    {
        ((CSliderCtrl *)GetDlgItem(IDC_VOLUME_SLIDER))->SetRange(0, 100);
        ((CEdit *)GetDlgItem(IDC_VOLUME_EDIT))->SetLimitText(3);
    }
    ((CSliderCtrl *)GetDlgItem(IDC_VOLUME_SLIDER))->SetPos(m_volume);
    SetDlgItemInt(IDC_VOLUME_EDIT, m_volume);

    // Head Size
    ((CSliderCtrl *)GetDlgItem(IDC_HEADSIZE_SLIDER))->SetRange(0, 100);
    ((CEdit *)GetDlgItem(IDC_HEADSIZE_EDIT))->SetLimitText(3);
    ((CSliderCtrl *)GetDlgItem(IDC_HEADSIZE_SLIDER))->SetPos(m_headSize);
    SetDlgItemInt(IDC_HEADSIZE_EDIT, m_headSize);

    // Pitch Fluctuation
    ((CSliderCtrl *)GetDlgItem(IDC_PITCHFLUCTUATION_SLIDER))->SetRange(0, 100);
    ((CEdit *)GetDlgItem(IDC_PITCHFLUCTUATION_EDIT))->SetLimitText(3);
    ((CSliderCtrl *)GetDlgItem(IDC_PITCHFLUCTUATION_SLIDER))->SetPos(m_pitchFluctuation);
    SetDlgItemInt(IDC_PITCHFLUCTUATION_EDIT, m_pitchFluctuation);

    // Roughness
    ((CSliderCtrl *)GetDlgItem(IDC_ROUGHNESS_SLIDER))->SetRange(0, 100);
    ((CEdit *)GetDlgItem(IDC_ROUGHNESS_EDIT))->SetLimitText(3);
    ((CSliderCtrl *)GetDlgItem(IDC_ROUGHNESS_SLIDER))->SetPos(m_roughness);
    SetDlgItemInt(IDC_ROUGHNESS_EDIT, m_roughness);

    // Breathiness
    ((CSliderCtrl *)GetDlgItem(IDC_BREATHINESS_SLIDER))->SetRange(0, 100);
    ((CEdit *)GetDlgItem(IDC_BREATHINESS_EDIT))->SetLimitText(3);
    ((CSliderCtrl *)GetDlgItem(IDC_BREATHINESS_SLIDER))->SetPos(m_breathiness);
    SetDlgItemInt(IDC_BREATHINESS_EDIT, m_breathiness);
}

void CVoicetunerDlg::SetModified(bool modified)
{
    bool enable;

    if (modified != m_modified)
    {
        // Update m_modified flag
        m_modified = modified;

        if (m_displayedVoice < m_numPresetVoices)
        {
            // Predefined voice, so Save button should be disabled
            enable = false;
        }
        else
        {
            // User-defined voice, so Save button should only be enabled if voice has been modified
            enable = m_modified;
        }

        // Enable/disable button
        GetDlgItem(IDC_SAVE)->EnableWindow(m_modified);
        if (!enable && GetFocus() == NULL)
        {
            // Focus was on Save button, but now disabled, so move focus
            GetDlgItem(IDC_SAVEAS)->SetFocus();
        }
    }
}
    
void CVoicetunerDlg::ActOnSampleRate(int sampleRate)
{
    if (SaveWarning())
    {
        // Set m_sampleRate
        m_sampleRate = sampleRate;

        // OK to proceed
        theApp.DoWaitCursor(1); // set wait cursor
        ActOnVoiceSelection(m_displayedVoice);
        theApp.DoWaitCursor(-1); // restore cursor
    }
    else
    {
        // Cancel sample rate change
        DisplaySampleRate();
    }
}

void CVoicetunerDlg::InitializeControls()
{
    int toFile;
    CVString fileName;
    int rc;

    // Set PropStorage path
    m_propStorage->SetPath(PATH_VOICE_TUNER);

#ifdef EMBEDDED
    // Disable sample rate and to file
    GetDlgItem(IDC_SAMPLERATE_8)->EnableWindow(FALSE);
    GetDlgItem(IDC_SAMPLERATE_11)->EnableWindow(FALSE);
    GetDlgItem(IDC_SAMPLERATE_22)->EnableWindow(FALSE);
#endif // EMBEDDED

    // Set m_toFile
    rc = m_propStorage->Get(KEY_TO_FILE, &toFile);
    if (rc != PROPSTORAGE_RC_OK)
    {
        // Not found
        toFile = 0;
    }
    CheckDlgButton(IDC_TOFILE, toFile);
    ActOnToFile(toFile ? true : false); // this will set m_toFile

    // Set m_fileName
    rc = m_propStorage->Get(KEY_FILE_NAME, &fileName);
    if (rc == PROPSTORAGE_RC_OK)
    {
        // Found
        SetDlgItemText(IDC_FILENAME_EDIT, fileName); // this will set m_fileName
    }
    else
    {
        // Not found
        m_fileName.Empty();
    }
}

void CVoicetunerDlg::UpdateSynthesizeOrStop()
{
    bool disable;

    // Disable button only if not synthesizing, m_toFile is true, and file name is empty
    disable = !m_synthesizing && m_toFile && m_fileName.IsEmpty();

    // Enable/disable button
    GetDlgItem(IDC_SYNTHESIZEORSTOP)->EnableWindow(!disable);
    if (disable && GetFocus() == NULL)
    {
        // Focus was on button, but now disabled, so move focus
        GetDlgItem(IDC_TOFILE)->SetFocus();
    }
}

void CVoicetunerDlg::ActOnEditChange(int edit, int slider, int *voiceParameterPtr)
{
    int value;
    int min, max;

    // Get edit value and slider range
    value = GetDlgItemInt(edit);
    ((CSliderCtrl *)GetDlgItem(slider))->GetRange(min, max);

    if (value >= min && value <= max)
    {
        if (value != *voiceParameterPtr)
        {
            // value is in slider range and has changed, so update voice parameter, slider, and m_modified flag
            *voiceParameterPtr = value;
            ((CSliderCtrl *)GetDlgItem(slider))->SetPos(value);
            SetModified(true);
        }
    }
    else
    {
        // value is out of slider range, so undo edit change
        SetDlgItemInt(edit, *voiceParameterPtr);
    }
}

void CVoicetunerDlg::ConvertToCodeString(const char *s, CString& codeString)
{
    size_t i;

    codeString.Empty();
    for (i = 0; i <= strlen(s); i++)
    {
        if (s[i] == '\\' || s[i] == '\"')
        {
            // Need escape character, so insert backslash
            codeString += '\\';
        }
        codeString += s[i];
    }
}

bool CVoicetunerDlg::SaveWarning()
{
    if (m_modified)
    {
        // Modified, so display warning
        if (MessageBox("Warning: You are about to lose unsaved voice parameters!", APP_NAME, MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK)
        {
            // OK
            return true;
        }
        else
        {
            // Cancel
            return false;
        }
    }
    else
    {
        // Not modified, so no warning necessary
        return true;
    }
}

void CVoicetunerDlg::ActOnSave(const char *voiceName)
{
    CString path;

    // Set m_propStorage path to "Voice Tuner/<language>/<voice>"
    path = PATH_VOICE_TUNER "/";
    path += LANGUAGES[m_languageIndex].paramString;
    path += "/";
    path += voiceName;
    m_propStorage->SetPath((LPCTSTR)path);

    // Save base voice, sample rate, and real world units
    m_propStorage->Set(KEY_BASE_VOICE, m_baseVoice);
    m_propStorage->Set(KEY_SAMPLE_RATE, m_sampleRate);
    m_propStorage->Set(KEY_REAL_WORLD_UNITS, m_realWorldUnits);

    // Save voice parameters
    m_propStorage->Set(KEY_GENDER, m_gender);
    m_propStorage->Set(KEY_PITCH_BASELINE, m_pitchBaseline);
    m_propStorage->Set(KEY_SPEED, m_speed);
    m_propStorage->Set(KEY_VOLUME, m_volume);
    m_propStorage->Set(KEY_HEAD_SIZE, m_headSize);
    m_propStorage->Set(KEY_PITCH_FLUCTUATION, m_pitchFluctuation);
    m_propStorage->Set(KEY_ROUGHNESS, m_roughness);
    m_propStorage->Set(KEY_BREATHINESS, m_breathiness);

    // Not modified
    SetModified(false);
}

void CVoicetunerDlg::CleanUp()
{
    // Free engine
    eciDelete(m_eciInstance);
#ifdef EMBEDDED
    ealClose();
#endif // EMBEDDED

    // Free PropStorage object
    m_propStorage->Release();

    // Free timer
    KillTimer(m_timer);
}

BEGIN_MESSAGE_MAP(CVoicetunerDlg, CDialog)
	//{{AFX_MSG_MAP(CVoicetunerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_LANGUAGE, OnSelchangeLanguage)
	ON_CBN_SELCHANGE(IDC_VOICE, OnSelchangeVoice)
	ON_BN_CLICKED(IDC_SAMPLERATE_8, OnSamplerate8)
	ON_BN_CLICKED(IDC_SAMPLERATE_11, OnSamplerate11)
	ON_BN_CLICKED(IDC_SAMPLERATE_22, OnSamplerate22)
	ON_BN_CLICKED(IDC_GENDER_MALE, OnGenderMale)
	ON_BN_CLICKED(IDC_GENDER_FEMALE, OnGenderFemale)
	ON_EN_KILLFOCUS(IDC_PITCHBASELINE_EDIT, OnKillfocusPitchbaselineEdit)
	ON_EN_KILLFOCUS(IDC_SPEED_EDIT, OnKillfocusSpeedEdit)
	ON_EN_KILLFOCUS(IDC_VOLUME_EDIT, OnKillfocusVolumeEdit)
	ON_EN_KILLFOCUS(IDC_HEADSIZE_EDIT, OnKillfocusHeadsizeEdit)
	ON_EN_KILLFOCUS(IDC_PITCHFLUCTUATION_EDIT, OnKillfocusPitchfluctuationEdit)
	ON_EN_KILLFOCUS(IDC_ROUGHNESS_EDIT, OnKillfocusRoughnessEdit)
	ON_EN_KILLFOCUS(IDC_BREATHINESS_EDIT, OnKillfocusBreathinessEdit)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_REALWORLDUNITS, OnRealworldunits)
	ON_BN_CLICKED(IDC_SYNTHESIZEORSTOP, OnSynthesizeOrStop)
	ON_BN_CLICKED(IDC_TOFILE, OnTofile)
	ON_EN_CHANGE(IDC_FILENAME_EDIT, OnChangeFilenameEdit)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CODE, OnCode)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoicetunerDlg message handlers

BOOL CVoicetunerDlg::OnInitDialog()
{
    int displayedLanguage;

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialize member variables
    if (!Initialize())
    {
        // Error, so exit
        EndDialog(IDCANCEL);
        return TRUE;
    }

    // Fill Language combobox
    if (!FillLanguage(&displayedLanguage))
    {
        // Error, so exit
        CleanUp();
        EndDialog(IDCANCEL);
        return TRUE;
    }

    // Fill controls for displayedLanguage
    ActOnLanguageSelection(displayedLanguage);

    // Initialize controls
    InitializeControls();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVoicetunerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVoicetunerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVoicetunerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVoicetunerDlg::OnTimer(UINT nIDEvent) 
{
    if (m_synthesizing && !eciSpeaking(m_eciInstance))
    {
        m_synthesizing = false;
        SetDlgItemText(IDC_SYNTHESIZEORSTOP, "S&ynthesize");
    }
	
	CDialog::OnTimer(nIDEvent);
}

void CVoicetunerDlg::OnSelchangeLanguage() 
{
    int displayedLanguage;

    // Get displayed language
    displayedLanguage = SendDlgItemMessage(IDC_LANGUAGE, CB_GETCURSEL, 0, 0);

    if (displayedLanguage != m_displayedLanguage)
    {
        // Language changed
        if (SaveWarning())
        {
            // OK to proceed
            theApp.DoWaitCursor(1); // set wait cursor
            SavePreviousVoice();
            ActOnLanguageSelection(displayedLanguage);
            theApp.DoWaitCursor(-1); // restore cursor
        }
        else
        {
            // Cancel language change
            SendDlgItemMessage(IDC_LANGUAGE, CB_SETCURSEL, m_displayedLanguage, 0);
        }
    }
}

void CVoicetunerDlg::OnSelchangeVoice() 
{
    int displayedVoice;

    // Get displayed voice
    displayedVoice = SendDlgItemMessage(IDC_VOICE, CB_GETCURSEL, 0, 0);

    if (displayedVoice != m_displayedVoice)
    {
        // Voice changed
        if (SaveWarning())
        {
            // OK to proceed
            theApp.DoWaitCursor(1); // set wait cursor
            ActOnVoiceSelection(displayedVoice);
            theApp.DoWaitCursor(-1); // restore cursor
        }
        else
        {
            // Cancel voice change
            SendDlgItemMessage(IDC_VOICE, CB_SETCURSEL, m_displayedVoice, 0);
        }
    }
}

void CVoicetunerDlg::OnSamplerate8() 
{
    ActOnSampleRate(0);
}

void CVoicetunerDlg::OnSamplerate11() 
{
    ActOnSampleRate(1);
}

void CVoicetunerDlg::OnSamplerate22() 
{
    ActOnSampleRate(2);
}

void CVoicetunerDlg::OnGenderMale() 
{
    // Changed to male
    m_gender = 0;
    SetModified(true);
}

void CVoicetunerDlg::OnGenderFemale() 
{
    // Changed to female
    m_gender = 1;
    SetModified(true);
}

void CVoicetunerDlg::OnKillfocusPitchbaselineEdit() 
{
    ActOnEditChange(IDC_PITCHBASELINE_EDIT, IDC_PITCHBASELINE_SLIDER, &m_pitchBaseline);
}

void CVoicetunerDlg::OnKillfocusSpeedEdit() 
{
    ActOnEditChange(IDC_SPEED_EDIT, IDC_SPEED_SLIDER, &m_speed);
}

void CVoicetunerDlg::OnKillfocusVolumeEdit() 
{
    ActOnEditChange(IDC_VOLUME_EDIT, IDC_VOLUME_SLIDER, &m_volume);
}

void CVoicetunerDlg::OnKillfocusHeadsizeEdit() 
{
    ActOnEditChange(IDC_HEADSIZE_EDIT, IDC_HEADSIZE_SLIDER, &m_headSize);
}

void CVoicetunerDlg::OnKillfocusPitchfluctuationEdit() 
{
    ActOnEditChange(IDC_PITCHFLUCTUATION_EDIT, IDC_PITCHFLUCTUATION_SLIDER, &m_pitchFluctuation);
}

void CVoicetunerDlg::OnKillfocusRoughnessEdit() 
{
    ActOnEditChange(IDC_ROUGHNESS_EDIT, IDC_ROUGHNESS_SLIDER, &m_roughness);
}

void CVoicetunerDlg::OnKillfocusBreathinessEdit() 
{
    ActOnEditChange(IDC_BREATHINESS_EDIT, IDC_BREATHINESS_SLIDER, &m_breathiness);
}

void CVoicetunerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    int value;
    int *voiceParameterPtr;
    int edit;

    if (nSBCode == SB_ENDSCROLL)
    {
        // Get new slider value, ID of corresponding edit control, and pointer to voice parameter
        switch (pScrollBar->GetDlgCtrlID())
        {
        case IDC_PITCHBASELINE_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_pitchBaseline;
            edit = IDC_PITCHBASELINE_EDIT;
            break;
        case IDC_SPEED_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_speed;
            edit = IDC_SPEED_EDIT;
            break;
        case IDC_VOLUME_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_volume;
            edit = IDC_VOLUME_EDIT;
            break;
        case IDC_HEADSIZE_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_headSize;
            edit = IDC_HEADSIZE_EDIT;
            break;
        case IDC_PITCHFLUCTUATION_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_pitchFluctuation;
            edit = IDC_PITCHFLUCTUATION_EDIT;
            break;
        case IDC_ROUGHNESS_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_roughness;
            edit = IDC_ROUGHNESS_EDIT;
            break;
        case IDC_BREATHINESS_SLIDER:
            value = ((CSliderCtrl *)pScrollBar)->GetPos();
            voiceParameterPtr = &m_breathiness;
            edit = IDC_BREATHINESS_EDIT;
            break;
        default:
            return;
        }

        // Update voice parameter, edit control, and modified flag
        if (value != *voiceParameterPtr)
        {
            *voiceParameterPtr = value;
            SetDlgItemInt(edit, value);
            SetModified(true);;
        }
    }
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVoicetunerDlg::OnRealworldunits() 
{
    // Set parameters using old units
    SetParameters();

    // Get displayed value
    if (IsDlgButtonChecked(IDC_REALWORLDUNITS))
    {
        m_realWorldUnits = 1;
    }
    else
    {
        m_realWorldUnits = 0;
    }

    // Convert voice parameters to new units
    eciSetParam(m_eciInstance, eciRealWorldUnits, m_realWorldUnits);
    GetVoiceParameters();
    DisplayVoiceParameters();

    // Update m_modified flag
    SetModified(true);
}

void CVoicetunerDlg::OnSynthesizeOrStop() 
{
    CEdit *testEdit;
    int lineCount, lineLength;
    char *testLine;
    int i;

    // Set wait cursor
    theApp.DoWaitCursor(1);

    if (!m_synthesizing) // synthesize
    {
        if (m_toFile)
        {
            // Output to file name
            eciSetOutputFilename(m_eciInstance, m_fileName);
        }
        else
        {
            // Output to default audio device
            eciSetOutputDevice(m_eciInstance, -1);
        }

        // Set parameters
        SetParameters();

        // Synthesize test phrase
        testEdit = (CEdit *)GetDlgItem(IDC_TEST);
        lineCount = testEdit->GetLineCount();
        for (i = 0; i < lineCount; i++)
        {
            lineLength = testEdit->LineLength(testEdit->LineIndex(i));
            testLine = (char *)malloc(max((lineLength + 1) * sizeof(char), sizeof(int)));
            testEdit->GetLine(i, testLine, lineLength);
            testLine[lineLength] = '\0';
            eciAddText(m_eciInstance, testLine);
            free(testLine);
        }
        eciSynthesize(m_eciInstance);

        // Set language and copy base voice in case they were changed by annotations
        eciSetParam(m_eciInstance, eciLanguageDialect, LANGUAGES[m_languageIndex].param);
        eciCopyVoice(m_eciInstance, m_baseVoice, 0);

        // Restore text mode, number mode, and dictionary settings in case they were changed by annotations
        eciSetParam(m_eciInstance, eciTextMode, 0);
        eciSetParam(m_eciInstance, eciNumberMode, 1);
        eciSetParam(m_eciInstance, eciDictionary, 0);

        // Now synthesizing
        m_synthesizing = true;
        SetDlgItemText(IDC_SYNTHESIZEORSTOP, "Stop S&ynthesis");
    }
    else // stop synthesis
    {
        // Stop synthesis
        eciStop(m_eciInstance);

        // No longer synthesizing
        m_synthesizing = false;
        SetDlgItemText(IDC_SYNTHESIZEORSTOP, "S&ynthesize");
    }

    // Update Synthesize/Stop button
    UpdateSynthesizeOrStop();

    // Restore cursor
    theApp.DoWaitCursor(-1);
}

void CVoicetunerDlg::OnTofile() 
{
    if (IsDlgButtonChecked(IDC_TOFILE))
    {
	    ActOnToFile(true);
    }
	else
    {
        ActOnToFile(false);
    }
}

void CVoicetunerDlg::OnChangeFilenameEdit() 
{
    // Set m_fileName
    GetDlgItemText(IDC_FILENAME_EDIT, m_fileName);

    // Update Synthesize/Stop button
    UpdateSynthesizeOrStop();
}

void CVoicetunerDlg::OnBrowse() 
{
    char workingDirectory[_MAX_PATH + 1];

    // Create Browse dialog and initialize with m_fileName
#ifdef EMBEDDED
    CFileDialog dlgBrowse(FALSE, "pcm", m_fileName, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, "Raw 16-bit Mono PCM Audio (*.pcm)|*.pcm||");
#else
    CFileDialog dlgBrowse(FALSE, "wav", m_fileName, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, "Windows PCM Sound (*.wav)|*.wav|u-law with Header (*.au)|*.au|Raw 8-bit Mono u-law without Header (*.rau)|*.rau||");
#endif // EMBEDDED

    // Set caption
    dlgBrowse.m_ofn.lpstrTitle = "Browse for File Name";

    // Set initial directory to the working directory
    GetCurrentDirectory(_MAX_PATH + 1, workingDirectory);
    dlgBrowse.m_ofn.lpstrInitialDir = workingDirectory;

    // Display dialog
    if (dlgBrowse.DoModal() == IDOK)
    {
        // User selected a valid filename, so update edit box
        m_fileName = dlgBrowse.GetPathName();
        SetDlgItemText(IDC_FILENAME_EDIT, m_fileName);
    }
}

void CVoicetunerDlg::OnSave() 
{
    CString voice;

	ActOnSave(m_voiceName);
}

void CVoicetunerDlg::OnSaveas() 
{
    CSaveasDlg dlgSaveas(this);
    int displayedVoice;

    if (dlgSaveas.DoModal() == IDOK)
    {
        // Save parameters for new voice
        ActOnSave(dlgSaveas.m_voiceName);

        // Refill Voice combobox
        FillVoice();

        // Select new voice
        displayedVoice = SelectUserDefinedVoice(dlgSaveas.m_voiceName);

        // Load selected voice
        ActOnVoiceSelection(displayedVoice);
    }
}

void CVoicetunerDlg::OnDelete() 
{
    CString warning, path;

    // Get confirmation from user
    warning.Format("Are you sure you want to delete %s voice '%s'?", LANGUAGES[m_languageIndex].name, (const char *)m_voiceName);
    if (MessageBox(warning, APP_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        // Delete voice
        path = PATH_VOICE_TUNER "/";
        path += LANGUAGES[m_languageIndex].paramString;
        m_propStorage->SetPath((LPCTSTR)path); // "Voice Tuner/<language>"
        m_propStorage->RemoveDir((LPCTSTR)m_voiceName);

        // Refill Voice combobox
        FillVoice();

        // Select first voice
        SendDlgItemMessage(IDC_VOICE, CB_SETCURSEL, 0, 0);

        // Load selected voice
        ActOnVoiceSelection(0);
    }
}

void CVoicetunerDlg::OnCode() 
{
	CCodeDlg codeDlg(this);
    codeDlg.DoModal();	
}

void CVoicetunerDlg::OnOK()
{
    // Must override this method due to MFC bug,
    // otherwise Enter key sometimes causes call to CDialog::OnOK()

    // See if Enter key was pressed while focus was on a voice parameter edit box
    switch (GetFocus()->GetDlgCtrlID())
    {
    case IDC_PITCHBASELINE_EDIT:
        ActOnEditChange(IDC_PITCHBASELINE_EDIT, IDC_PITCHBASELINE_SLIDER, &m_pitchBaseline);
        break;
    case IDC_SPEED_EDIT:
        ActOnEditChange(IDC_SPEED_EDIT, IDC_SPEED_SLIDER, &m_speed);
        break;
    case IDC_VOLUME_EDIT:
        ActOnEditChange(IDC_VOLUME_EDIT, IDC_VOLUME_SLIDER, &m_volume);
        break;
    case IDC_HEADSIZE_EDIT:
        ActOnEditChange(IDC_HEADSIZE_EDIT, IDC_HEADSIZE_SLIDER, &m_headSize);
        break;
    case IDC_PITCHFLUCTUATION_EDIT:
        ActOnEditChange(IDC_PITCHFLUCTUATION_EDIT, IDC_PITCHFLUCTUATION_SLIDER, &m_pitchFluctuation);
        break;
    case IDC_ROUGHNESS_EDIT:
        ActOnEditChange(IDC_ROUGHNESS_EDIT, IDC_ROUGHNESS_SLIDER, &m_roughness);
        break;
    case IDC_BREATHINESS_EDIT:
        ActOnEditChange(IDC_BREATHINESS_EDIT, IDC_BREATHINESS_SLIDER, &m_breathiness);
        break;
    }

    //return CDialog::OnOK();
}

void CVoicetunerDlg::OnCancel() 
{
    if (SaveWarning())
    {
        // Set wait cursor
        theApp.DoWaitCursor(1);

        // Save version, output to file, file name, language, and voice
        m_propStorage->SetPath(PATH_VOICE_TUNER); // "Voice Tuner"
        m_propStorage->Set(KEY_VERSION, VERSION);
        m_propStorage->Set(KEY_TO_FILE, m_toFile ? 1 : 0);
        m_propStorage->Set(KEY_FILE_NAME, (LPCTSTR)m_fileName);
        m_propStorage->Set(KEY_LANGUAGE, LANGUAGES[m_languageIndex].paramString);
        SavePreviousVoice();

        // Clean up
        CleanUp();

        // Execute cancel operation
	    CDialog::OnCancel();

        // Restore cursor
        theApp.DoWaitCursor(-1);
    }
}

BOOL CVoicetunerDlg::OnHelpInfo(HELPINFO* /*pHelpInfo*/) 
{
    // Disable Help
	return TRUE;
	
	//return CDialog::OnHelpInfo(pHelpInfo);
}
