package com.ibm.collaboration.realtime.tts.preferences;

import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;

import com.ibm.collaboration.realtime.tts.Common;
import com.ibm.collaboration.realtime.tts.TTSMessageHandler;
import com.ibm.collaboration.realtime.tts.TTSPlugin;
import com.ibm.collaboration.realtime.tts.msgs.Messages;

/**
 * This class represents a preference page that
 * is contributed to the Preferences dialog. By 
 * subclassing <samp>FieldEditorPreferencePage</samp>, we
 * can use the field support built into JFace that allows
 * us to create a page that is small and knows how to 
 * save, restore and apply itself.
 * <p>
 * This page is used to modify preferences only. They
 * are stored in the preference store that belongs to
 * the main plug-in class. That way, preferences can
 * be accessed directly via the preference store.
 */

public class TTSPreferencePage	extends PreferencePage implements IWorkbenchPreferencePage, SelectionListener {
	
	final String [] radioStrings = new String[] {
			PreferenceConstants.P_STARTUP_SAPI5,
			PreferenceConstants.P_STARTUP_SAPI4,
			PreferenceConstants.P_STARTUP_ECI
	};
	final Button [] radioButtons = new Button[radioStrings.length];
	
/*	private Button buttonStartupSAPI4;
	private Button buttonStartupSAPI5;
	private Button buttonStartupECI;
*/	
//	private Button buttonGenderMale;
//	private Button buttonGenderFemale;
	private Button buttonReadBack, buttonAlertMsg, buttonSuppressLargeWord;
	
	private Button playButton, configButton, aboutButton;//, readbackCheck;
	private static int selection = 0;	// 1->SAPI5, 2->SAPI4, 3->ECI
	private Text testMessage;
	private Group group2;
	private Spinner charThreshold;	
	
	public TTSPreferencePage() {
		super();
		// Set the preference store for the preference page.
		setPreferenceStore(TTSPlugin.getDefault().getPreferenceStore());
		setDescription(Messages.getString("TTSPreferencePage.7"));
	}	
	
	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {		
	}	
	protected void performApply() {
//		super.performApply();
		storeSettings();				
	}
	public boolean performOk() {
		storeSettings();
		clean();
		return true;
	}
	
	public boolean performCancel() {
		clean();
		return true;
	}				
	
	protected void performDefaults() {
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
				
		testMessage.setText (store.getDefaultString(PreferenceConstants.OPTION_TEST_MSG));
		buttonAlertMsg.setSelection(store.getDefaultBoolean(PreferenceConstants.OPTION_NOTIFY_ALERT));
		buttonReadBack.setSelection(store.getDefaultBoolean(PreferenceConstants.OPTION_NOTIFY_READBACK));
		buttonSuppressLargeWord.setSelection(store.getDefaultBoolean(PreferenceConstants.OPTION_SUPPRESS));
		charThreshold.setValues(store.getDefaultInt(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD),10,100,0,1,10);		
		charThreshold.setEnabled(buttonSuppressLargeWord.getSelection());
		
//		getDefaultsButton().setEnabled(false);
//		getApplyButton().setEnabled(false);		
		//super.performDefaults();
	}

	protected void clean() {
		for (int i=0;i<radioStrings.length;i++){
			radioButtons[i].removeSelectionListener(this);
		}
		this.dispose();
	}
	
	//Play the sound
	private void PlayIt(String text) {
		if(selection==0) return;
/*		TTSPlugin.TTSUninit();
		
		Common.TTSStatus = TTSPlugin.setRuntime(1<<(selection-1)*2);
		TTSPlugin.TTSInit();
		if(selection==3) 
			PreferenceDialog.SetLanguageVoice(
					getPreferenceStore().getInt(PreferenceConstants.OPTION_LANG),
					getPreferenceStore().getInt(PreferenceConstants.OPTION_ECI_VOICE),
					getPreferenceStore().getInt(PreferenceConstants.OPTION_SAMPLERATE)
					);
		else if(selection ==2)
			PreferenceDialog.SetLanguageVoice(
					getPreferenceStore().getInt(PreferenceConstants.OPTION_SAPI4_VOICE)
					, -1, -1);
*/		
		TTSMessageHandler.TTSSpeak(text);
	}
	
	protected Control createContents(Composite parent) {
//		final ScrolledComposite sc = new ScrolledComposite(parent, SWT.H_SCROLL | SWT.V_SCROLL);
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		final Composite composite = new Composite(parent, SWT.NONE);
	    composite.setLayout(new GridLayout(1, false));
	    
		Group group=new Group(composite,SWT.SHADOW_ETCHED_IN);
		group.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		group.setText(Messages.getString("TTSPreferencePage.0")); //$NON-NLS-1$
		group.setLayout(new GridLayout(1, false));
		
/*		sc.setContent(composite);	    
	    sc.setExpandHorizontal(true);
	    sc.setExpandVertical(true);
	    sc.addControlListener(new ControlAdapter() {
	        public void controlResized(ControlEvent e) {
	            Rectangle r = sc.getClientArea();
	            sc.setMinSize(composite.computeSize(r.width, SWT.DEFAULT));
	          }	    	
	    });
*/	    
		//The preference UI
		//Composite Settings = new Composite(group, SWT.BORDER);	
		for (int i=0;i<radioStrings.length;i++){
			radioButtons[i] = new Button (group,SWT.RADIO);
			radioButtons[i].setText(radioStrings[i]);
			radioButtons[i].addSelectionListener(this);
			if( ((Common.AvailableTTS >> (i*2)) & 1) ==0 )	//check available TTS status
				radioButtons[i].setEnabled(false);
		}

		Composite subcomposite = new Composite(group, SWT.NONE);
		subcomposite.setLayout(new GridLayout(2, true));	
		
		configButton = new Button (subcomposite, SWT.PUSH);
		configButton.setText (Messages.getString("TTSPreferencePage.4")); //$NON-NLS-1$
		configButton.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_CENTER));
		configButton.addSelectionListener(this);		

		aboutButton = new Button (subcomposite, SWT.PUSH);
		aboutButton.setText (Messages.getString("TTSPreferencePage.8")); //$NON-NLS-1$		
		aboutButton.addSelectionListener(this);
		
		//Second panel
		group2=new Group(composite,SWT.SHADOW_ETCHED_IN);
		group2.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		group2.setText(Messages.getString("TTSPreferencePage.10")); //$NON-NLS-1$
		group2.setLayout(new GridLayout(2, false));
		
/*		gridLayout = new GridLayout ();
		gridLayout.numColumns = 1;
		group.setLayout (gridLayout);
*/
/*		buttonGenderMale = new Button (group2, SWT.RADIO);
		buttonGenderMale.setText (PreferenceConstants.P_GENDER_MALE);

		buttonGenderFemale = new Button (group2, SWT.RADIO);
		buttonGenderFemale.setText (PreferenceConstants.P_GENDER_FEMALE);
*/		
		buttonSuppressLargeWord = new Button(group2, SWT.CHECK); //new BooleanFieldEditor(PreferenceConstants.OPTION_SUPPRESS, Messages.getString("TTSPreferencePage.6"), group2); //$NON-NLS-1$ 
		buttonSuppressLargeWord.setText(Messages.getString("TTSPreferencePage.6"));
		buttonSuppressLargeWord.setSelection(store.getBoolean(PreferenceConstants.OPTION_SUPPRESS));		
			
		charThreshold = new Spinner(group2, SWT.SINGLE|SWT.BORDER);
		buttonSuppressLargeWord.addSelectionListener(new SelectionListener(){

			public void widgetDefaultSelected(SelectionEvent arg0) {				
			}

			public void widgetSelected(SelectionEvent arg0) {
				charThreshold.setEnabled(buttonSuppressLargeWord.getSelection());				
			}			
		});
		
		//Due to https://bugs.eclipse.org/bugs/show_bug.cgi?id=139871, we use setValues instead of setDigits
//		charThreshold.setDigits(store.getInt(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD));
		charThreshold.setValues(store.getInt(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD),10,100,0,1,10);
		charThreshold.setEnabled(buttonSuppressLargeWord.getSelection());
			
		buttonAlertMsg = new Button(group2, SWT.CHECK); //new BooleanFieldEditor(PreferenceConstants.OPTION_NOTIFY_ALERT, Messages.getString("TTSPreferencePage.9"), group2);
/*		fieldAlertMsg.fillIntoGrid(group2,1);
		fieldAlertMsg.setPreferenceStore(getPreferenceStore());
		fieldAlertMsg.load();*/
		buttonAlertMsg.setText(Messages.getString("TTSPreferencePage.9"));
		buttonAlertMsg.setLayoutData(new GridData(GridData.FILL, GridData.FILL,true, true,2, 1));
		buttonAlertMsg.setSelection(store.getBoolean(PreferenceConstants.OPTION_NOTIFY_ALERT));
		
		buttonReadBack = new Button(group2, SWT.CHECK);//new BooleanFieldEditor(PreferenceConstants.OPTION_NOTIFY_READBACK, Messages.getString("TTSPreferencePage.3"), group2);
		buttonReadBack.setText(Messages.getString("TTSPreferencePage.3"));
		buttonReadBack.setLayoutData(new GridData(GridData.FILL, GridData.FILL,true, true,2, 1));
		buttonReadBack.setSelection(store.getBoolean(PreferenceConstants.OPTION_NOTIFY_READBACK));
		
		Group group3=new Group(composite,SWT.SHADOW_ETCHED_IN);
		group3.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		group3.setText(Messages.getString("TTSPreferencePage.11")); //$NON-NLS-1$
		group3.setLayout(new GridLayout(2, false));
	    
		testMessage = new Text (group3, SWT.BORDER);		
		testMessage.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));			

		playButton = new Button (group3, SWT.PUSH);
		playButton.setText (Messages.getString("TTSPreferencePage.5")); //$NON-NLS-1$
		playButton.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_CENTER));
		playButton.addSelectionListener(this);
		
/*		if(selection==0)
		{
			testMessage.setEnabled(false);
			playButton.setEnabled(false);
		}
*/		
//		AutoStartButton = new Button (group2, SWT.CHECK);
//		AutoStartButton.setText("Don't automatically active when Sametime starting"); 
/*		FormToolkit a;
		Hyperlink link = new Hyperlink(composite, 0);
		link.addHyperlinkListener(new HpyerAdapter() {	
			public void linkActivated(HyperlinkEvent e) {
				System.out.println("Link activated!");
			}

			public void linkEntered(HyperlinkEvent arg0) {
			}

			public void linkExited(HyperlinkEvent arg0) {
			}

		});
		link.setText("A sample link");*/		
		initSettings();
		return composite;
	}

	private void initSettings() {
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		
		if(Common.AvailableTTS == 0) { 
			playButton.setEnabled(false);
			configButton.setEnabled(false);
			testMessage.setEnabled(false);
//			readbackCheck.setEnabled(false);
			buttonAlertMsg.setEnabled(false);
			buttonReadBack.setEnabled(false);			
			buttonSuppressLargeWord.setEnabled(false);
		} else {
	//		AutoStartButton.setSelection(store.getBoolean(PreferenceConstants.P_STARTUP));
			testMessage.setText(store.getString(PreferenceConstants.OPTION_TEST_MSG));
			//selection = store.getInt(PreferenceConstants.OPTION_RUNTIMES);
			if(Common.IsDebug) System.out.println("initSettings:selection="+selection);
			
			if (Common.TTSStatus== Common.TTS_SAPI5_STARTED) {
				selection = 1;
				configButton.setEnabled(false);
				aboutButton.setEnabled(false);
			}
			else if (Common.TTSStatus == Common.TTS_SAPI4_STARTED)
				selection = 2;
			else if (Common.TTSStatus == Common.TTS_IBM_STARTED) {
				selection = 3;
				aboutButton.setEnabled(false);
			}
			else 
				selection = 0;
			
			if(selection>0) {
				radioButtons[selection-1].setSelection(true);				
			} else {			
				for( int i=0;i<radioStrings.length;i++) {	//set default selection
					if (radioButtons[i].getEnabled()) {
						radioButtons[i].setSelection(true);
						selection = i+1;
						break;
					}
				}				
			}
			PlayIt(Messages.getString("TTSPreferencePage.13"));
	//		TTSPlugin.TTS_ReadBack = store.getBoolean(PreferenceConstants.TTS_READBACK);
//			if(TTSPlugin.TTS_ReadBack)
//				readbackCheck.setSelection(true);
			//mGoodbye.setText(store.getString(PreferenceConstants.GOODBYE));
			//mDefaultVoice.setSelection(new StructuredSelection(store.getString(PreferenceConstants.DEFAULT_VOICE)));			
		}
	}
	
	private void storeSettings() {
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		
		store.setValue(PreferenceConstants.OPTION_TEST_MSG, testMessage.getText());
//		if(TTSPlugin.IsDebug) TTSPlugin.logger.log(Level.INFO,"storeSettings:selection="+selection);
		
//		TTSPlugin.TTS_ReadBack = readbackCheck.getSelection();
//		if(TTSPlugin.IsDebug) TTSPlugin.logger.log(Level.INFO,"storeSettings:TTS_ReadBack ="+TTSPlugin.TTS_ReadBack);		
		Common.OPTION_READBACK = buttonReadBack.getSelection();
		Common.OPTION_ANNOUNCEMENT = buttonAlertMsg.getSelection();
		Common.OPTION_SUPPRESS = buttonSuppressLargeWord.getSelection();
		Common.OPTION_SUPPRESS_THRESHOLD = charThreshold.getSelection();

		store.setValue(PreferenceConstants.OPTION_NOTIFY_ALERT, buttonAlertMsg.getSelection());
		store.setValue(PreferenceConstants.OPTION_NOTIFY_READBACK, buttonReadBack.getSelection());		
		store.setValue(PreferenceConstants.OPTION_SUPPRESS, buttonSuppressLargeWord.getSelection());
		store.setValue(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD, charThreshold.getSelection());
		
		if(selection >0) {
			
			//TTS runtime will not be restarted if no change.
			int old_selection = store.getInt(PreferenceConstants.OPTION_RUNTIMES);			  
			if(old_selection != selection) {
				store.setValue(PreferenceConstants.OPTION_RUNTIMES, selection);
				
				TTSPlugin.TTSUninit();			
				Common.TTSStatus = TTSPlugin.setRuntime(1<<(selection-1)*2);			
				TTSPlugin.TTSInit();
			}
			
			//But runtime configuration will be set always.
			if(selection==3) 
				PreferenceDialog.SetLanguageVoice(
						getPreferenceStore().getInt(PreferenceConstants.OPTION_LANG),
						getPreferenceStore().getInt(PreferenceConstants.OPTION_ECI_VOICE),
						getPreferenceStore().getInt(PreferenceConstants.OPTION_SAMPLERATE)
						);
			else if(selection ==2)
				PreferenceDialog.SetLanguageVoice(
						getPreferenceStore().getInt(PreferenceConstants.OPTION_SAPI4_VOICE)
						, -1, -1);	
			
			//Change the button status
			configButton.setEnabled(selection == 2 || selection == 3					
					? true : false);
			aboutButton.setEnabled(selection == 2 
					? true : false);
		}
	}	

	public void widgetDefaultSelected(SelectionEvent s) {
		widgetSelected(s);
	}
	public void widgetSelected(SelectionEvent s) {
//		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		
		for (int i=0;i<radioStrings.length;i++)
		{
			if(s.widget==radioButtons[i]) {
				selection = i+1;
/*				configButton.setEnabled(radioButtons[i].getText() == PreferenceConstants.P_STARTUP_ECI ||
										radioButtons[i].getText() == PreferenceConstants.P_STARTUP_SAPI4
										? true : false);
				aboutButton.setEnabled(radioButtons[i].getText() == PreferenceConstants.P_STARTUP_SAPI4 
										? true : false);*/
				break;
			}
		}
	
		testMessage.setEnabled(selection >0);	//if selection>0, button enabled 
		playButton.setEnabled(selection >0);			
		
		if(s.widget == playButton) {
			String text=testMessage.getText();
			if(text.length()>0)	PlayIt(text);
		} else if (s.widget == configButton) {
			if(selection == 3 || selection == 2) {
				PlayIt(Messages.getString("TTSPreferencePage.4"));
				PreferenceDialog dlg=new PreferenceDialog(getShell());
				dlg.setTitle(Messages.getString("TTSPreferencePage.4"));
				if(dlg.open(selection)) {}				
			}
				
		} else if (s.widget == aboutButton) {
			TTSPlugin.TTSDialog(false);
		}
	}
}