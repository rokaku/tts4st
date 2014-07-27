package com.ibm.collaboration.realtime.tts;

//To share the common methods and variables
public abstract class Common {
	public static native String [] Enum();
	
	// TTS status flag
	public static final int TTS_NOTFOUND= 0; 
	public static final int TTS_SAPI5_STARTED= 0x01; 
	public static final int TTS_SAPI5_STOPPED= 0x02; 
	public static final int TTS_SAPI4_STARTED= 0x04; 
	public static final int TTS_SAPI4_STOPPED= 0x08; 
	public static final int TTS_IBM_STARTED= 0x10; 
	public static final int TTS_IBM_STOPPED= 0x20; 
	
	public static boolean OPTION_ANNOUNCEMENT;
	public static boolean OPTION_READBACK;
	public static boolean OPTION_SUPPRESS;
	public static int OPTION_SUPPRESS_THRESHOLD;
	public static boolean IsDebug;	

	public static int TTSStatus = Common.TTS_NOTFOUND;	//for current TTS status
	public static int AvailableTTS = Common.TTS_NOTFOUND;	//for available TTS state
	
	public final static String PROPERTY_SAPI5 = "runtime.sapi5.enable";
	public final static String PROPERTY_SAPI4 = "runtime.sapi4.enable";
	public final static String PROPERTY_ECI = "runtime.eci.enable";
}
