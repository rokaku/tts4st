package com.ibm.collaboration.realtime.tts;

import com.ibm.collaboration.realtime.messages.MessageHandler;
import com.ibm.collaboration.realtime.messages.MessageHandlerAdapter;

/**
 * Callback handler of incoming messages that monitors 
 * incoming text messages before they are passed to
 * the registered message handlers.
 * 
 * @see com.ibm.collaboration.realtime.messages.MessageHandlerListener extension point.
 */

public class TTSMessageHandlerAdapter extends MessageHandlerAdapter {

	public TTSMessageHandlerAdapter(MessageHandler handler) {
		super(handler);
	}
	
	public TTSMessageHandlerAdapter() {
		super(new TTSMessageHandler());
	}
	
}
