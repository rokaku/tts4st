//Created by Charles Lu 


class CBufNotify : public ITTSBufNotifySink {
    private:
    public:
        CBufNotify(void);
        ~CBufNotify(void);
        
        void NotifyOutput( const /*TCHAR*/char *str );
        
        // IUnknown members
        STDMETHODIMP         QueryInterface (REFIID, LPVOID FAR *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
                        
        // ITTSNotifySink
        STDMETHOD (BookMark)        (QWORD, DWORD);
        STDMETHOD (TextDataDone)    (QWORD, DWORD);
        STDMETHOD (TextDataStarted) (QWORD);
        STDMETHOD (WordPosition)    (QWORD, DWORD);
};
typedef CBufNotify * PCBufNotify;

// globals
PCBufNotify	g_pIBufNotifySink = NULL;  // buf notification object


typedef struct
{
	BOOL bAttribChanged;
	BOOL bAudioStart;
	BOOL bAudioStop;
	BOOL bVisual;
	BOOL bBookMark;
	BOOL bTextDataDone;
	BOOL bTextDataStarted;
	BOOL bWordPosition;
} ITTSNotify_t;

typedef struct
{
	BOOL        bPaused;
	BOOL        bPosTracking;
	ITTSNotify_t Not;
} EngData_t;

typedef EngData_t  *PENGDATA;

#define MAX_ENUMMODES ARRAYMAXSIZE
//Global variables for SAPI4
PITTSCENTRAL    g_pITTSCentral = NULL;         // engine interface
PITTSDIALOGS    g_pITTSDialogs = NULL;     //for SAPI dialogs	
PITTSENUM   g_pITTSEnum = NULL;
LPUNKNOWN   g_pIUnkAudio = NULL;

PENGDATA    gpEng=NULL;               // global pointer to current engine data struct
TTSMODEINFO gaTTSInfo[MAX_ENUMMODES];
//extern PITTSATTRIBUTES gpITTSAttributes;	// global attributes interface pointer 
