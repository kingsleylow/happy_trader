/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtRtData;


/**
 *
 * @author DanilinS
 */
public interface CommunicationInterface {

	public void onStartLengthyOperation(String operationName);
	public void onStopLengthyOperation();
	public void onStartCommunicationChannel() throws Exception;
	public void onStopCommunicationChannel();

	public void onLogEntryReceived(boolean isHistory, CommonLog clog);
	public void onDrawableObjectReceived(boolean isHistory, HtDrawableObject cdraw);
	public void onHtRtDataReceived(boolean isHistory, HtRtData rtdata);
	public void onErrorOccured(String message);
	public void onHeartBeat();
	
}
