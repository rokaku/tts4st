package com.ibm.collaboration.realtime.tts;

import java.util.logging.Level;

import com.ibm.collaboration.realtime.imhub.actions.BuddyListAction;
import com.ibm.collaboration.realtime.tts.msgs.Messages;

/**
 * Action contributed to the main window (buddy list).
 */

public class GlobalAction extends BuddyListAction {

	public GlobalAction() {
		super(null);
		if(Common.AvailableTTS == Common.TTS_NOTFOUND) 			
			this.setVisible(false);	// =disable
/*		else if (TTSPlugin.TTSStatus == TTSPlugin.TTS_IBM_STOPPED ||		//to update the icon status
				 TTSPlugin.TTSStatus == TTSPlugin.TTS_SAPI4_STOPPED ||
				 TTSPlugin.TTSStatus == TTSPlugin.TTS_SAPI5_STOPPED) {
			setToolTipText(Messages.getString("ListAction.1")); //$NON-NLS-1$
			setImageDescriptor(TTSPlugin.getImageDescriptor("images/unmute.png"));
		}
*/		
		//if(TTSPlugin.logger.isLoggable(Level.WARNING))  TTSPlugin.logger.log(Level.WARNING, "ListAction()");
	}
	
	/*public void setEnable(boolean enableit) {
		if(enableit)
			setImageDescriptor(TTSPlugin.getImageDescriptor("images/TTSTalk.gif"));
		else
			setImageDescriptor(TTSPlugin.getImageDescriptor("images/TTSTalkDisable.gif"));
	}*/
	
	public void run() {
		//System.err.println(">>>ListAction:run");
		
		//if(TTSPlugin.TTSStatus!=TTSPlugin.TTS_NOTFOUND) {
		if(Common.AvailableTTS >0) {			
			if( Common.TTSStatus == Common.TTS_IBM_STARTED ||
				Common.TTSStatus == Common.TTS_SAPI4_STARTED ||
				Common.TTSStatus == Common.TTS_SAPI5_STARTED ) {
				TTSPlugin.TTSUninit();				
				setToolTipText(Messages.getString("ListAction.0")); //$NON-NLS-1$
				Common.TTSStatus<<=1;
//				if(TTSPlugin.IsDebug) TTSPlugin.logger.log(Level.INFO,"ListAction.TTSStatus:"+TTSPlugin.TTSStatus);
				//setEnable(false);
				setImageDescriptor(TTSPlugin.getImageDescriptor("images/mute.png"));								 //$NON-NLS-1$
			} else {
				TTSPlugin.TTSInit();				
				setToolTipText(Messages.getString("ListAction.1")); //$NON-NLS-1$
				Common.TTSStatus>>=1;
				//TTSPlugin.TTSStatus=TTSPlugin.TTS_IBM_STARTED;				
				//setEnable(true);
//				if(TTSPlugin.IsDebug) TTSPlugin.logger.log(Level.INFO,"ListAction.TTSStatus:"+TTSPlugin.TTSStatus);
				setImageDescriptor(TTSPlugin.getImageDescriptor("images/unmute.png")); //$NON-NLS-1$
			}
		} 
	}	
}
