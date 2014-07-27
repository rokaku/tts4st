package com.ibm.collaboration.realtime.tts.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

import com.ibm.collaboration.realtime.tts.TTSPlugin;
import com.ibm.collaboration.realtime.tts.msgs.Messages;

/**
 * Class used to initialize default preference values.
 */
public class PreferenceInitializer extends AbstractPreferenceInitializer {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer#initializeDefaultPreferences()
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = TTSPlugin.getDefault().getPreferenceStore();
		
		store.setDefault(PreferenceConstants.OPTION_TEST_MSG, 
				Messages.getString("TTSPreferencePage.2"));
		store.setDefault(PreferenceConstants.OPTION_NOTIFY_READBACK, false);
		store.setDefault(PreferenceConstants.OPTION_NOTIFY_ALERT, true);
		store.setDefault(PreferenceConstants.OPTION_SUPPRESS, true);
		store.setDefault(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD, 15);
	}

}
