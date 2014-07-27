package com.ibm.collaboration.realtime.tts;

import com.ibm.collaboration.realtime.messages.AlertUserMessage;
import com.ibm.collaboration.realtime.messages.DefaultMessageHandler;
import com.ibm.collaboration.realtime.messages.FileReceivedMessage;
import com.ibm.collaboration.realtime.messages.Message;
import com.ibm.collaboration.realtime.messages.ShowBalloonAlertMessage;
import com.ibm.collaboration.realtime.messages.im.ImMessage;
import com.ibm.collaboration.realtime.messages.im.ImTextReceivedMessage;
import com.ibm.collaboration.realtime.messages.im.ImTextSendMessage;
import com.ibm.collaboration.realtime.messages.im.ManyToManyTextReceivedMessage;
import com.ibm.collaboration.realtime.people.PeopleService;
import com.ibm.collaboration.realtime.people.Person;
import com.ibm.collaboration.realtime.servicehub.ServiceException;
import com.ibm.collaboration.realtime.servicehub.ServiceHub;
import com.ibm.collaboration.realtime.tts.msgs.Messages;
import com.ibm.collaboration.realtime.ui.alert.AlertOptions;

/**
 * Message handler that monitors incoming text looking
 * for possible substitution of registered acronyms.
 */

public class TTSMessageHandler extends DefaultMessageHandler {

	public static native boolean TTSSpeak(String message);
	
	public TTSMessageHandler() {
		//logger = Logger.getLogger(TTSPlugin.PLUGIN_ID);
	}	
/*	
	protected String ParseXML(String xml) {		
		String msg = null;
		DocumentBuilderFactory domFactory = DocumentBuilderFactory.newInstance();
		try {
			DocumentBuilder domBuilder = domFactory.newDocumentBuilder();
			Document doc=domBuilder.parse(xml);
			Element root = doc.getDocumentElement();
			NodeList nodes = root.getChildNodes();
			if(nodes != null) {
				for (int i=0;i<nodes.getLength();i++) {
					Node node= nodes.item(i);
					if(node.getNodeType()==Node.ELEMENT_NODE)
						TTSPlugin.logger.log(Level.INFO, "ParseXML:"+node.getAttributes());
				}
			}
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		return msg;		
	}
*/	

	protected String GetName(ImMessage message) {
		String partnerID = message.getPartnerUniqueId();	//after complete file transferring job the PartnerID will return null
		if (partnerID != null)
			partnerID = getPersonById(partnerID).getDisplayName() +" "+ Messages.getString("TTSMessageHandler.1"); //$NON-NLS-1$
		return partnerID;
	}
	private Person getPersonById(String id) {
		Person person = null;
		try {
			PeopleService peopleSvc = (PeopleService) ServiceHub.getService(PeopleService.SERVICE_TYPE);
			person = peopleSvc.getPersonById(id);
		} catch (ServiceException e) {
			e.printStackTrace();
		}		
		return person;
	}
	
/*	private String GetName(String partnerID) {
		int index = partnerID.indexOf('@');
		if (index>=0)
			partnerID = partnerID.substring(0, index);
		return partnerID;
	}	
*/	
	//SAPI 5 is not required to remove tag 
	protected String RemoveTag(String message) {
		String text= message;
		int index=0, index2=0;		

		while (text.indexOf("<") != -1 && text.indexOf(">") != -1) {
			index = text.indexOf("<");
			index2= text.indexOf(">");
			if(index<index2) {
				if(index>0) 
					text = text.substring(0, index) + text.substring(index2+1, text.length());
				else
					text = text.substring(index2+1, text.length());
			}				
		}

/*		while (text.indexOf("<") != -1 && text.indexOf(">") != -1) { //$NON-NLS-1$ //$NON-NLS-2$
			index = text.indexOf(">"); //$NON-NLS-1$
			text=text.substring(index+1, text.length());
			index = text.lastIndexOf("<"); //$NON-NLS-1$
			text=text.substring(0, index);
		}
*/		return text;
	}
	
/*	public void handleMessage(ManyToManyTextSendMessage message) {
		String text = message.getText();
		if (text != null && Common.NOTIFY_ReadBack) {
			text = RemoveTag(text);
			
			if(text.length() > 0) {
				text = Messages.getString("TTSMessageHandler.2") + text; //$NON-NLS-1$
				TTSSpeak(text);			
			}
		}
	}*/
	
	public void handleMessage(ManyToManyTextReceivedMessage message) {
		//super.handleMessage(message);
		String text = message.getText();
		if (text != null) {
			//if(TTSPlugin.logger.isLoggable(Level.FINEST))  TTSPlugin.logger.log(Level.INFO, "ManyToMany Original ChatText:"+text);
			text = RemoveTag(text);			
			if(Common.OPTION_SUPPRESS) text = SuppressLongWord(text, Common.OPTION_SUPPRESS_THRESHOLD);
			if(text.length() > 0 && text.indexOf("&#")==-1) {
				text = GetName(message) + text;
				try {
					Thread.sleep(1000);	//Delay 1 second
				} catch (InterruptedException e) {}
				TTSSpeak(text);
			}
		}
	}
	
	public void handleMessage(ImTextReceivedMessage message) {
		String text = message.getText();
		if (text != null) {
			
			//String partnerID = message.getPartnerContactId();
			//text = GetName(partnerID) + " says, "+text;
			if(Common.IsDebug)  System.out.println("Original ChatText:"+text); //$NON-NLS-1$
			
			text = RemoveTag(text);
			if(Common.IsDebug) System.out.println("RemoveTag:"+text);
			if(Common.OPTION_SUPPRESS) text = SuppressLongWord(text, Common.OPTION_SUPPRESS_THRESHOLD);
			if(Common.IsDebug) System.out.println("SuppressLongWord:"+text);
			if(text.length() > 0 && text.indexOf("&#")==-1) {
				String partnerID = GetName(message);
				if (partnerID!=null)	text = partnerID + text;
				try {
					Thread.sleep(1000);	//Delay 1 second
				} catch (InterruptedException e) {}
				TTSSpeak(text);
			}
		}
	}
	
	public void handleMessage(ImTextSendMessage message) {
		String text = message.getText();
		if (text != null && Common.OPTION_READBACK) {
			if(Common.IsDebug)  System.out.println("Original send ChatText:"+text);					 //$NON-NLS-1$
				
//			TTSPlugin.logger.log(Level.INFO, "getPartnerAlias:"+message.getPartnerAlias());
//			TTSPlugin.logger.log(Level.INFO, "getPartnerContactId:"+message.getPartnerContactId());
//			TTSPlugin.logger.log(Level.INFO, "getCommunityId:"+message.getCommunityId());
//			TTSPlugin.logger.log(Level.INFO, "getConversationId:"+message.getConversationId());
//			TTSPlugin.logger.log(Level.INFO, "getLocalAlias:"+message.getLocalAlias());
//			TTSPlugin.logger.log(Level.INFO, "getSourceId:"+message.getSourceId());
//			TTSPlugin.logger.log(Level.INFO, "getPartnerDisplayName:"+message.getPartnerDisplayName());

			text = RemoveTag(text);
			if(Common.IsDebug) System.out.println("RemoveTag:"+text);
			if(Common.OPTION_SUPPRESS) text = SuppressLongWord(text, Common.OPTION_SUPPRESS_THRESHOLD);
			if(Common.IsDebug) System.out.println("SuppressLongWord:"+text);
			if(text.length() > 0) {
				text = Messages.getString("TTSMessageHandler.2") + text; //$NON-NLS-1$				
				TTSSpeak(text);
			}
		}
	}
	
	private String SuppressLongWord(String message, int charThread) {
		String text = "";
		String tokens[]= message.split(" +");
		for(int i=0;i<tokens.length;i++) {
			if(tokens[i].length()<=charThread)
				text+=tokens[i]+" ";			
		}
		
		return text.trim();
	}

	public void handleDefaultMessage(Message message) {
	}

	public void handleMessage(AlertUserMessage event) {				
		if(Common.OPTION_ANNOUNCEMENT) {
			String message = ((AlertOptions)event.getAlertOptions()).getMessage();
			if(Common.OPTION_SUPPRESS) message = SuppressLongWord(message, Common.OPTION_SUPPRESS_THRESHOLD);
			TTSSpeak(message);
		}			
	}

	public void handleMessage(FileReceivedMessage message) {
		String filename = message.getFileName();
		//Doesn't work
	}

	public void handleMessage(ShowBalloonAlertMessage message) {
		String msg = message.getMessageEventCodeString();
		//Doesn't work		
	}
}
