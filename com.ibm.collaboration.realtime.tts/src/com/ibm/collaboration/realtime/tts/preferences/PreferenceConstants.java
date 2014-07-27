package com.ibm.collaboration.realtime.tts.preferences;

/**
 * Constant definitions for plug-in preferences
 */
public class PreferenceConstants {

	public static final String P_PATH = "pathPreference";

	public static final String P_BOOLEAN = "booleanPreference";

	public static final String P_CHOICE = "choicePreference";

	public static final String P_STRING = "stringPreference";

	//public static final String P_STARTUP = PreferenceConstants.class.getPackage().getName()+".startup";	
	public static final String P_STARTUP_SAPI4 = "Microsof SAPI 4.x compatible TTS runtimes";
	public static final String P_STARTUP_SAPI5 = "Microsof SAPI 5.x compatible TTS runtimes";
	public static final String P_STARTUP_ECI = "IBM TTS runtimes";
	
//	public static final String P_GENDER = "genderPreference";
//	public static final String P_GENDER_MALE = "Male";
//	public static final String P_GENDER_FEMALE = "Female";
	
	public static final String OPTION_TEST_MSG = PreferenceConstants.class.getPackage().getName()+".testmsg";
	public static final String OPTION_RUNTIMES = PreferenceConstants.class.getPackage().getName()+".runtimes";
	public static final String OPTION_NOTIFY_READBACK = PreferenceConstants.class.getPackage().getName()+".notify.readback";
	public static final String OPTION_NOTIFY_ALERT = PreferenceConstants.class.getPackage().getName()+".notify.alert";
	public static final String OPTION_SUPPRESS = PreferenceConstants.class.getPackage().getName()+".suppress";
	public static final String OPTION_SUPPRESS_THRESHOLD = PreferenceConstants.class.getPackage().getName()+".suppress.threshold";
	public static final String OPTION_LANG = PreferenceConstants.class.getPackage().getName()+".language";
	public static final String OPTION_ECI_VOICE = PreferenceConstants.class.getPackage().getName()+".eci.voice";
	public static final String OPTION_SAPI4_VOICE = PreferenceConstants.class.getPackage().getName()+".sapi4.voice";
	public static final String OPTION_SAMPLERATE = PreferenceConstants.class.getPackage().getName()+".samplerate";
}
