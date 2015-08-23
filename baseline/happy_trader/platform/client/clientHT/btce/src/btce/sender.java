/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package btce;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import org.apache.commons.codec.binary.Hex;
import org.apache.http.HttpEntity;
import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;
import org.json.*; //JSON data format //available at http://www.java2s.com/Code/JarDownload/java/java-json.jar.zip



/**
 *
 * @author Victor_Zoubok
 */
public class sender {

		private static long _nonce;
		private static final String _secret = "f6e7296cfc05d47707dd4f7984c022be3e5d2125e064da9a0bdc135c73d19f0f";
		private static final String _key = "UEWN3HEZ-PPO4LUOV-A8WG85ZI-0NE27G58-909S03XR";
	

		static {
				// Create a unixtime nonce for the new API.
				_nonce  = System.currentTimeMillis();
		}
		/**
		 * Execute a authenticated query on btc-e.
		 *
		 * @param method The method to execute.
		 * @param arguments The arguments to pass to the server.
		 *
		 * @return The returned data as JSON or null, if the request failed.
		 *
		 * @see http://pastebin.com/K25Nk2Sv
		 */
		public final static JSONObject authenticatedHTTPRequest(String method, Map<String, String> arguments) throws Exception
		{
				//HashMap<String, String> headerLines = new HashMap<String, String>();  // Create a new map for the header lines.
				Mac mac;
				SecretKeySpec key = null;
				List <NameValuePair> headerLines = new ArrayList <NameValuePair>();

				if (arguments == null) {  
						// If the user provided no arguments, just create an empty argument array.
						arguments = new HashMap<String, String>();
				}

				arguments.put("method", method);  // Add the method to the post data.
				arguments.put("nonce", "" + ++_nonce);  // Add the dummy nonce.

				String postData = "?";

				for (Iterator argumentIterator = arguments.entrySet().iterator(); argumentIterator.hasNext();) {
						Map.Entry argument = (Map.Entry) argumentIterator.next();

						if (postData.length() > 1) {
								postData += "&";
						}
						postData += argument.getKey() + "=" + argument.getValue();
				}

				// Create a new secret key
				try {
						key = new SecretKeySpec(_secret.getBytes("UTF-8"), "HmacSHA512");
				} catch (UnsupportedEncodingException uee) {
						System.err.println("Unsupported encoding exception: " + uee.toString());
						return null;
				}

				// Create a new mac
				try {
						mac = Mac.getInstance("HmacSHA512");
				} catch (NoSuchAlgorithmException nsae) {
						System.err.println("No such algorithm exception: " + nsae.toString());
						return null;
				}

				// Init mac with key.
				try {
						mac.init(key);
				} catch (InvalidKeyException ike) {
						System.err.println("Invalid key exception: " + ike.toString());
						return null;
				}

				// Add the key to the header lines.
				headerLines.add(new BasicNameValuePair("Key", _key));

				// Encode the post data by the secret and encode the result as base64.
				try {
						headerLines.add(new BasicNameValuePair("Sign", Hex.encodeHexString(mac.doFinal(postData.getBytes("UTF-8")))));
				} catch (UnsupportedEncodingException uee) {
						System.err.println("Unsupported encoding exception: " + uee.toString());
						return null;
				}

				// Now do the actual request
				//String requestResult = HttpUtils.httpPost("https://btc-e.com/tapi", headerLines, postData);
				
				CloseableHttpClient httpclient = HttpClients.createDefault();
				
				HttpPost httpPost = new HttpPost("https://btc-e.com/tapi");
				UrlEncodedFormEntity encodedEntity = new UrlEncodedFormEntity(headerLines);
				
				ByteArrayOutputStream ou = new ByteArrayOutputStream();
				encodedEntity.writeTo(ou);
				String f_s = new String( ou.toByteArray());
							
				httpPost.setHeader(HTTP.CONTENT_TYPE, "application/x-www-form-urlencoded; charset=UTF-8");
    		httpPost.setEntity(encodedEntity);
				CloseableHttpResponse response2 = httpclient.execute(httpPost);
				
				HttpEntity entity2 = response2.getEntity();
			
					
				String requestResult =EntityUtils.toString(entity2, "utf-8");
				
				if (requestResult != null) {   // The request worked

						try {
								/*
								// Convert the HTTP request return value to JSON to parse further.
								JSONObject jsonResult = JSONObject.fromObject( requestResult);

								// Check, if the request was successful
								int success = ((Number)jsonResult.get("success")).intValue();

								if (success == 0) {  // The request failed.
										String errorMessage = (String)jsonResult.get("error");

										System.err.println("btc-e.com trade API request failed: " + errorMessage);

										return null;
								} else {  // Request succeeded!
										return (JSONObject)jsonResult.get("return");
								}*/

						} catch (Exception je) {
								System.err.println("Cannot parse json request result: " + je.toString());

								return null;  // An error occured...
						}
				}

				return null;  // The request failed.
		}
}
