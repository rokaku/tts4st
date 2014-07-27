package com.ibm.collaboration.realtime.tts.preferences;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;

import com.ibm.collaboration.realtime.tts.TTSPlugin;
import com.ibm.collaboration.realtime.tts.msgs.Messages;

public class PreferenceDialog extends Dialog {
	public static native String [] GetLanguages(int displayedLanguage);
	public static native boolean SetLanguageVoice(int Language, int Voice, int SampleRate);
	private static Combo comboLang;
	private static Combo comboVoice;	
	private String strTitle;
	private boolean result;
	
	public PreferenceDialog(Shell parent) {
		super(parent, 0);
		result = false;
	}

	public void setTitle(String strTitle) {
		this.strTitle = strTitle;
	}
	
/*	protected void configureShell(Shell shell) {
		super.configureShell(shell);		
		shell.setText("Voice Settings");
	}
*/
//	protected Control createContents(Composite parent) {
	public boolean open(int selection) {
		boolean result;
		String[] lang=GetLanguages(selection);

		//Prepare for the UI
		Shell parent = getParent();
		final Shell shell = new Shell(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
		if (strTitle != null) shell.setText(strTitle);		
		shell.setLayout(new GridLayout(2, false));
		
		//Draw corresponding dialog
		if(selection == 2)
			result = SAPI4Dialog(shell, lang);
		else
			result = ECIDialog(shell, lang);
		
		shell.pack();
		shell.open();
		
		Display display = parent.getDisplay();
		while (!shell.isDisposed()) {
			if (!display.readAndDispatch()) display.sleep();			
		}	
		return result;
	}
	private boolean SAPI4Dialog(final Shell shell, String [] VoiceName) {
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		
		new Label (shell, SWT.NONE).setText (Messages.getString("PreferenceDialog.4")); //$NON-NLS-1$
		
		comboLang = new Combo (shell, SWT.READ_ONLY);
		for(int i=0;VoiceName[i]!=null && i<VoiceName.length;i++) 
			comboLang.add(VoiceName[i]);			
		comboLang.select(store.getInt(PreferenceConstants.OPTION_SAPI4_VOICE));
		
		Button button4 = new Button (shell, SWT.PUSH);
		button4.setText (Messages.getString("PreferenceDialog.1")); //$NON-NLS-1$
		button4.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent s) {
				SetLanguageVoice(comboLang.getSelectionIndex(),-1,-1);
				IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
				store.setValue(PreferenceConstants.OPTION_SAPI4_VOICE, comboLang.getSelectionIndex());
				result = true;
				shell.dispose();
			}
		});
		
		Button button5 = new Button (shell, SWT.PUSH);
		button5.setText (Messages.getString("PreferenceDialog.2")); //$NON-NLS-1$
		button5.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent s) {
				shell.dispose();
			}
		});		
		return result;
	}
	private boolean ECIDialog(final Shell shell, String [] LanguageName) {		
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();		
	
		new Label (shell, SWT.NONE).setText (Messages.getString("PreferenceDialog.3")); //$NON-NLS-1$
		
		comboLang = new Combo (shell, SWT.READ_ONLY);
		for(int i=0;LanguageName[i]!=null && i<LanguageName.length ;i++) 
			comboLang.add(LanguageName[i]);			
		
//		comboLang.select(0);
		
		new Label (shell, SWT.NONE).setText (Messages.getString("PreferenceDialog.4")); //$NON-NLS-1$

//		String[] voice=GetVoices();
		comboVoice = new Combo (shell, SWT.READ_ONLY);
//		for(int i=0;i<voice.length && voice[i]!=null;i++)
//			combo3.add(voice[i]);
		comboVoice.setItems (new String [] {Messages.getString("PreferenceDialog.5"), 
				Messages.getString("PreferenceDialog.6"), 
				Messages.getString("PreferenceDialog.7"), 
				Messages.getString("PreferenceDialog.8"), 
				Messages.getString("PreferenceDialog.9")}); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$ //$NON-NLS-4$ //$NON-NLS-5$
		//combo3.setText ("combo3");
//		comboVoice.select(0);

/*		RadioGroupFieldEditor rfe = new RadioGroupFieldEditor("myRadioGroup",
		        "Radio Group", 2, new String[][] { { "First Value", "first" },
		            { "Second Value", "second" },
		            { "Third Value", "third" },
		            { "Fourth Value", "fourth" } }, shell,
		        true);		
*///		    addField(rfe);
		
		new Label (shell, SWT.NONE).setText (Messages.getString("PreferenceDialog.10")); //$NON-NLS-1$
		Composite composite = new Composite(shell, SWT.NONE);
		//composite.setLayout(new GridLayout(2, false));
		composite.setLayout(new RowLayout()); 
		
		final String[] ratings = new String[] { "8", "11", "22"}; //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$
		final Button[] radios = new Button[ratings.length];
	    for (int i = 0; i < ratings.length; i++) {
		      radios[i] = new Button(composite, SWT.RADIO);
		      radios[i].setText(ratings[i]);		     
		}
	    int sampleRate = store.getInt(PreferenceConstants.OPTION_SAMPLERATE);
	    radios[sampleRate].setSelection(true);
		
		comboLang.select(store.getInt(PreferenceConstants.OPTION_LANG));
		comboVoice.select(store.getInt(PreferenceConstants.OPTION_ECI_VOICE));
		
		Button button4 = new Button (shell, SWT.PUSH);
		button4.setText (Messages.getString("PreferenceDialog.1")); //$NON-NLS-1$
		button4.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent s) {
				int sampleRate = 0;
				if(radios[1].getSelection()) 
					sampleRate = 1;
				else if (radios[2].getSelection())
					sampleRate = 2;
				
				SetLanguageVoice(comboLang.getSelectionIndex(), comboVoice.getSelectionIndex(), sampleRate);
				IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
				store.setValue(PreferenceConstants.OPTION_LANG, comboLang.getSelectionIndex());
				store.setValue(PreferenceConstants.OPTION_ECI_VOICE, comboVoice.getSelectionIndex());
				store.setValue(PreferenceConstants.OPTION_SAMPLERATE, sampleRate);				
//				close();
				result = true;
				shell.dispose();
			}
		});
		
		Button button5 = new Button (shell, SWT.PUSH);
		button5.setText (Messages.getString("PreferenceDialog.2")); //$NON-NLS-1$
		button5.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent s) {
//				close();
				shell.dispose();
			}
		});
		
//		parent.pack();		
		return result;
	}
	
	

}
