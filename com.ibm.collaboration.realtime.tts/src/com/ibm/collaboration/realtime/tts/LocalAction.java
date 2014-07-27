package com.ibm.collaboration.realtime.tts;

import com.ibm.collaboration.realtime.chat.actions.ChatWindowAction;

/**
 * Drop down menu that displays the list of possible
 * responses or the pop-up add on.
 * 
 * @see QrPopupAddOn
 */

public class LocalAction extends ChatWindowAction {
	public LocalAction() {
		if (Common.TTSStatus == Common.TTS_NOTFOUND)
			this.setVisible(false);
	}
	
	public void run() {	
		if(Common.TTSStatus != Common.TTS_NOTFOUND) {
			TTSPlugin.TTSStop(false);
		/*	setEnabled(true);
		} else if(TTSPlugin.eciState == TTSPlugin.TTS_STOPPED) {
			//setImageDescriptor(TTSPlugin.getImageDescriptor("images/TTSTalkDisable.gif"));
			setEnabled(false);
		} */				
	}
	
	//org.eclipse.swt.internal.win32.OS.OleInitialize (0);

	}
}