// voicetunerdlg.h : header file
//

#if !defined(AFX_VOICETUNERDLG_H__6D974E26_59FF_4699_9202_E32B0CA06806__INCLUDED_)
#define AFX_VOICETUNERDLG_H__6D974E26_59FF_4699_9202_E32B0CA06806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <eci.h>
//#include "propstorage.h"

#define APP_NAME "Voice Tuner" // for message box caption

//using namespace ViaVoice;

/////////////////////////////////////////////////////////////////////////////
// CVoicetunerDlg dialog

class CVoicetunerDlg : public CDialog
{
// Construction
public:
	CVoicetunerDlg(CWnd* pParent = NULL);	// standard constructor
    bool VoiceNameExists(const char *voiceName);
    void GetCode(CString& code);

// Dialog Data
	//{{AFX_DATA(CVoicetunerDlg)
	enum { IDD = IDD_VOICETUNER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoicetunerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVoicetunerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeLanguage();
	afx_msg void OnSelchangeVoice();
	afx_msg void OnSamplerate8();
	afx_msg void OnSamplerate11();
	afx_msg void OnSamplerate22();
	afx_msg void OnGenderMale();
	afx_msg void OnGenderFemale();
	afx_msg void OnKillfocusPitchbaselineEdit();
	afx_msg void OnKillfocusSpeedEdit();
	afx_msg void OnKillfocusVolumeEdit();
	afx_msg void OnKillfocusHeadsizeEdit();
	afx_msg void OnKillfocusPitchfluctuationEdit();
	afx_msg void OnKillfocusRoughnessEdit();
	afx_msg void OnKillfocusBreathinessEdit();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRealworldunits();
	afx_msg void OnSynthesizeOrStop();
	afx_msg void OnTofile();
	afx_msg void OnChangeFilenameEdit();
	afx_msg void OnBrowse();
	afx_msg void OnSave();
	afx_msg void OnSaveas();
	afx_msg void OnDelete();
	afx_msg void OnCode();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    UINT m_timer;
    IPropStorage *m_propStorage;
    ECIHand m_eciInstance;
    bool m_synthesizing;
    int m_displayedLanguage;
    int m_languageIndex;
    int m_numPresetVoices;
    CVStringArray m_voices;
    int m_displayedVoice;
    int m_baseVoice;
    int m_sampleRate;
    CVString m_voiceName;
    bool m_modified;
    int m_realWorldUnits;
    int m_gender;
    int m_pitchBaseline;
    int m_speed;
    int m_volume;
    int m_headSize;
    int m_pitchFluctuation;
    int m_roughness;
    int m_breathiness;
    bool m_toFile;
    CString m_fileName;

    bool Initialize();
    void InitializeEngine(int previousLanguageIndex);
    bool GetTTSDirectory(CString *ttsDirectory);
    void GetPreviousLanguageIndex(int *previousLanguageIndex);
    void ActOnToFile(bool toFile);
    bool FillLanguage(int *displayedLanguage);
    void ActOnLanguageSelection(int displayedLanguage);
    void SavePreviousVoice();
    void FillVoice();
    int SelectUserDefinedVoice(const char *voiceName);
    void ActOnVoiceSelection(int displayedVoice);
    void SetParameters();
    void GetVoiceParameters();
    void DisplaySampleRate();
    void DisplayVoiceParameters();
    void SetModified(bool modified);
    void ActOnSampleRate(int sampleRate);
    void InitializeControls();
    void UpdateSynthesizeOrStop();
    void ActOnEditChange(int edit, int slider, int *voiceParameterPtr);
    void ConvertToCodeString(const char *s, CString& codeString);
    bool SaveWarning();
    void ActOnSave(const char *voiceName);
    void CleanUp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICETUNERDLG_H__6D974E26_59FF_4699_9202_E32B0CA06806__INCLUDED_)
