/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils;

import java.lang.reflect.Array;
import org.apache.http.message.AbstractHttpMessage;

/**
 *
 * @author DanilinS
 */
public class Utils {
	public static void initHeaders(AbstractHttpMessage httpmethod) {
		httpmethod.setHeader("Accept-Language", "ru,ja;q=0.5");
		httpmethod.setHeader("Accept-Encoding", "deflate");
		httpmethod.setHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; InfoPath.1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)");

	}

	public  static <T> T[] castObjectArray(Class<T> clazz, Object[] src)
	{
		if (src == null)
			return null;

		T[] result = (T[])Array.newInstance(clazz, src.length);
		for(int i = 0; i < src.length; i++) {
			result[i] = (T)src[i];
		}
		
		return result;
		
	}

	public String createSignallerStructureForMainOrder(
			String symbol,
			boolean isLocalInstall,
			double issuePrice,
			String orderPlace,
			String orderMethod,
			double volume,
			String add_order_info,
			int order_type,
			String orderUidS,
			String description
	) throws Exception
	{

		StringBuilder custom_data = new StringBuilder();
		custom_data.append("SYMBOL:").append(symbol).append("\n");
		custom_data.append("UID:").append(orderUidS).append("\n");
		custom_data.append("ORDER_TYPE:").append(order_type).append("\n");
		custom_data.append("ORDER_METHOD:").append(orderMethod).append("\n");
		custom_data.append("ORDER_PLACE:").append(orderPlace).append("\n");
		custom_data.append("ISSUE_PRICE:").append(String.valueOf(issuePrice)).append("\n");
		custom_data.append("VOLUME:").append(String.valueOf(volume)).append("\n");
		custom_data.append("ADD_ORDER_INFO:").append(add_order_info).append("\n");
		custom_data.append("DESCRIPTION: [").append(description).append("]\n");
		custom_data.append("REGISTER_LOCALLY:").append(isLocalInstall ? "true" : "false").append("\n");

		return custom_data.toString();
	}



}
