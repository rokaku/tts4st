package com.ibm.collaboration.realtime.tts;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.ibm.collaboration.realtime.tts.preferences.PreferenceDialog;
import com.ibm.collaboration.realtime.tts.preferences.PreferenceConstants;

import java.io.FileInputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * The activator class controls the plug-in life cycle
 */
public class TTSPlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = TTSPlugin.class.getPackage().getName();	
	
	// The shared instance
	private static TTSPlugin plugin;
		
    public static Logger logger;
    private static Properties properties;
    
    public static native boolean TTSInit();
	public static native boolean TTSUninit();
	public static native boolean TTSStop(boolean state);
	public static native int setRuntime(int nTTS);
	public static native int TTSDetect(int Selection);
	public static native boolean TTSDialog(boolean isAboutDlg);
		
	static {
		/*	Load the native library */	
        logger = Logger.getLogger(PLUGIN_ID);
		try {
			System.loadLibrary("TTS");	
		} catch (UnsatisfiedLinkError e) {
			System.err.println("TTS Plugin DLL not found!");
		}
	
		//load property file
	    properties = new Properties();
	    IPath path = new Path("options.ini");
	    URL u=FileLocator.find(Platform.getBundle(PLUGIN_ID), path, null);
	    
	    if (u != null) { 
	    	try {
				u = FileLocator.resolve(u);
				properties.load(new FileInputStream(u.getFile()));
			} catch (IOException e) {
				e.printStackTrace();
			} 
	    }	   	
	}
	
	/**
	 * The constructor
	 */
	public TTSPlugin() {
		plugin = this;
	}

	protected int loadSettings() {
		int results=0;
		if(properties.getProperty(Common.PROPERTY_SAPI5, "true").equalsIgnoreCase("true"))
			results |= Common.TTS_SAPI5_STARTED;

		if(properties.getProperty(Common.PROPERTY_SAPI4, "true").equalsIgnoreCase("true"))
			results |= Common.TTS_SAPI4_STARTED;

		if(properties.getProperty(Common.PROPERTY_ECI, "true").equalsIgnoreCase("true"))
			results |= Common.TTS_IBM_STARTED;
		
		//Common.IsDebug = new Boolean(System.getProperty("tts.debug")).booleanValue();
		Common.IsDebug = new Boolean(properties.getProperty("tts.debug", "false")).booleanValue();
		
		if(Common.IsDebug) System.out.println("Loading TTS Settings: " + results);
		return results;
	}
	
	//mapping from 0-3 to TTS status flag
	public int Mapping(int selection) {
		return 1<<(selection-1)*2;
	}
	
	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		//load data
		//dectect SAPI/ECI
		//get preferencestore and set one of them enabled from settings						
		Common.AvailableTTS = TTSDetect(loadSettings());
		if(Common.IsDebug) System.out.println("TTS Runtimes status:" + Common.AvailableTTS);
						
		//Compare to user preference
		super.start(context);	//have to call this prior to call getPreferenceStore()	
		IPreferenceStore store = getPreferenceStore();
		int selection = store.getInt(PreferenceConstants.OPTION_RUNTIMES);
		Common.OPTION_READBACK = store.getBoolean(PreferenceConstants.OPTION_NOTIFY_READBACK);
		Common.OPTION_ANNOUNCEMENT = store.getBoolean(PreferenceConstants.OPTION_NOTIFY_ALERT);
		Common.OPTION_SUPPRESS = store.getBoolean(PreferenceConstants.OPTION_SUPPRESS);
		Common.OPTION_SUPPRESS_THRESHOLD = store.getInt(PreferenceConstants.OPTION_SUPPRESS_THRESHOLD);
		if(Common.OPTION_SUPPRESS_THRESHOLD==0) Common.OPTION_SUPPRESS_THRESHOLD=15;
		
//		if(Common.IsDebug) System.out.println("TTS Readback:"+ Common.OPTION_READBACK);
			
		if(selection >0) {
			if( ((1<<(selection-1)*2) & Common.AvailableTTS) > 0 )
				selection = 1<<(selection-1)*2;	//Map to TTS Status flag
			else	
				selection = Common.AvailableTTS;	
		} else
			selection = Common.AvailableTTS;	
		
		Common.TTSStatus = setRuntime(selection);	//Set 1st match TTS runtimes
			
/*		if(store.getBoolean(PreferenceConstants.OPTION_AUTOSTART) == false)	//user don't want to auto-start it
			TTSStatus<<=1;
		else
*/		{
			TTSInit();	//initialize runtime env.
			if(Common.TTSStatus == Common.TTS_IBM_STARTED) 
				PreferenceDialog.SetLanguageVoice(		
						store.getInt(PreferenceConstants.OPTION_LANG),
						store.getInt(PreferenceConstants.OPTION_ECI_VOICE),
						store.getInt(PreferenceConstants.OPTION_SAMPLERATE));
			else if (Common.TTSStatus == Common.TTS_SAPI4_STARTED)
				PreferenceDialog.SetLanguageVoice(
						store.getInt(PreferenceConstants.OPTION_SAPI4_VOICE), 
						-1, -1);
	
			if (Common.TTSStatus == Common.TTS_SAPI5_STARTED || Common.TTSStatus == Common.TTS_SAPI4_STARTED)
				TTSMessageHandler.TTSSpeak(" ");	//For SAPI something has to be spoken at first, don't know the reason
		}		
///		if (store.getBoolean(PreferenceConstants.OPTION_AUTOSTART))
///			TTSPlugin.TTSStatus>>=1;
		if(Common.IsDebug) logger.log(Level.INFO, "TTS_AutoStart:"+store.getBoolean(PreferenceConstants.OPTION_SUPPRESS));						
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		if(	Common.TTSStatus == Common.TTS_SAPI5_STOPPED ||
			Common.TTSStatus == Common.TTS_SAPI4_STOPPED ||
			Common.TTSStatus == Common.TTS_IBM_STOPPED) {
			
			//to auto save the latest status?
//			IPreferenceStore store = getPreferenceStore();
//			store.setValue(PreferenceConstants.OPTION_AUTOSTART, false);
		}		
		
		TTSUninit();
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static TTSPlugin getDefault() {
		return plugin;
	}
	
	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path.
	 *
	 * @param path the path
	 * @return the image descriptor
	 */	
	public static ImageDescriptor getImageDescriptor(String path) {
		return AbstractUIPlugin.imageDescriptorFromPlugin(PLUGIN_ID, path);
	}	
}