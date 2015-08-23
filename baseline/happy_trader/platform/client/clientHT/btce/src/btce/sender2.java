/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package btce;

/**
 * @author Antares If you find this sample useful, please support me~ BTC:
 * 1LVr2H9htN174nbY1fEUNE2mJedcpj2MUo LTC: LQPTTjsgVPZpkWgJkMfRPForhY5DK1AHXf
 */
import javax.crypto.Mac; //encode and decode
import javax.crypto.spec.SecretKeySpec;
import javax.net.ssl.HttpsURLConnection; //Https connection
import java.net.*;
import java.io.*;
import java.util.HashMap;
import org.apache.commons.codec.binary.Hex;
import org.json.*; //JSON data format //available at http://www.java2s.com/Code/JarDownload/java/java-json.jar.zip

public class sender2 {

		private static String APIKey = "UEWN3HEZ-PPO4LUOV-A8WG85ZI-0NE27G58-909S03XR";
		private static String Secret = "f6e7296cfc05d47707dd4f7984c022be3e5d2125e064da9a0bdc135c73d19f0f";
		private static String URLBase = "https://btc-e.com/tapi";
		private static String API2URLBase = "https://btc-e.com/api/2/";

		private String BuildQueryString(HashMap<String, String> args) {
		//imports needed:
				//import java.net.*;

				String QueryString = new String();

				for (String hashkey : args.keySet()) {
						if (QueryString.length() > 0) {
								QueryString += '&';
						}
						try {
								QueryString += URLEncoder.encode(hashkey, "UTF-8") + "="
									+ URLEncoder.encode(args.get(hashkey), "UTF-8");
						} catch (UnsupportedEncodingException e) {
								e.printStackTrace();
						}

				}//end for

				return QueryString;
		}

		private String SignRequest(String Request) {
		//imports needed:
				//import javax.crypto.Mac;
				//import javax.crypto.spec.SecretKeySpec;

				String SignedRequest = "";

				try {
						Mac HMAC = Mac.getInstance("HmacSHA512");
						byte [] byteSecret = Secret.getBytes();

						//byte[] byteSecret = Hex.decodeHex(Secret.toCharArray());
						HMAC.init(new SecretKeySpec(byteSecret, "HmacSHA512"));
						//SignedRequest = bytesToHex(HMAC.doFinal(Request.getBytes("UTF-8")));
						SignedRequest = Hex.encodeHexString(HMAC.doFinal(Request.getBytes("UTF-8")));
				} catch (java.security.NoSuchAlgorithmException e) {
						e.printStackTrace();
				} catch (java.security.InvalidKeyException e) {
						e.printStackTrace();
				} catch (UnsupportedEncodingException e) {
						e.printStackTrace();
				} catch (Exception e) {
						e.printStackTrace();
				}

				return SignedRequest.toUpperCase();
		}

		//this is the function for personal information (API key needed)
		public JSONObject executeQuery(String APIMethod, HashMap<String, String> args) {
		//imports needed:
				//import javax.net.ssl.HttpsURLConnection;
				JSONObject JSONAnswer = new JSONObject();

				try {
						args.put("nonce", Nonce());
						args.put("method", APIMethod);
						String post_data = BuildQueryString(args);
						String signature = SignRequest(post_data);

						URL Url = new URL(URLBase);
						HttpsURLConnection connection = (HttpsURLConnection) Url.openConnection();

						connection.setRequestMethod("POST");

						//write header
						connection.setRequestProperty("Key", APIKey);
						connection.setRequestProperty("Sign", signature.toLowerCase()); //it is important to use lower case

						connection.setDoOutput(true);
						connection.setDoInput(true);
						
					

						//Read the response
						DataOutputStream os = new DataOutputStream(connection.getOutputStream());
					
						os.writeBytes(post_data);
						os.close();

						BufferedReader br = new BufferedReader(new InputStreamReader((connection.getInputStream())));

						String Answer;
						if ((Answer = br.readLine()) != null) {
								JSONAnswer = new JSONObject(Answer);
						}
						br.close();
				} catch (java.io.FileNotFoundException e) {
				} catch (MalformedURLException e) {
						e.printStackTrace();
				} catch (IOException e) {
						e.printStackTrace();
				} catch (JSONException e) {
						e.printStackTrace();
				}

				return JSONAnswer;
		}

		//this is the function for market information (NO API key needed)
		public JSONObject GetData(String Pair, String Target) {
		//import java.net.*;
				//import java.io.*;
				JSONObject JSONAnswer = new JSONObject();

				String URLString = API2URLBase + Pair + "/" + Target;

				try {
						URL Url = new URL(URLString);
						java.net.URLConnection URLConn = Url.openConnection();
						HttpsURLConnection Connection = (HttpsURLConnection) URLConn;
						BufferedReader br = new BufferedReader(new InputStreamReader(Connection.getInputStream()));

						String Answer;
						if ((Answer = br.readLine()) != null) {
								JSONAnswer = new JSONObject(Answer);
						}
						br.close();
				} catch (MalformedURLException e) {
						e.printStackTrace();
				} catch (IOException e) {
						e.printStackTrace();
				} catch (JSONException e) {
						e.printStackTrace();
				}

				return JSONAnswer;
		}

	//==========Supporting Functions=================
	//Convert a byte[] to Hex string
		//see http://stackoverflow.com/questions/9655181/convert-from-byte-array-to-hex-string-in-java
		final protected static char[] hexArray = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

		private static String bytesToHex(byte[] bytes) {
				char[] hexChars = new char[bytes.length * 2];
				int v;
				for (int j = 0; j < bytes.length; j++) {
						v = bytes[j] & 0xFF;
						hexChars[j * 2] = hexArray[v >>> 4];
						hexChars[j * 2 + 1] = hexArray[v & 0x0F];
				}
				return new String(hexChars);
		}

		//Create a nonce number
		private String Nonce() {
				//long trimmer = 1375000000000L;
				//long MilliSec = System.currentTimeMillis() - trimmer;
				//return Long.toString(MilliSec);
				long MilliSec = System.currentTimeMillis() / 1000;
				return Long.toString(MilliSec);
				
		}

//	below are examples of how to use the functions to get data
//	import java.util.HashMap;
//	import org.json.*;
//
//	public class Main
//	{
//		public static void main(String[] args)
//		{
//			MtGox mg=new MtGox();
//			BTCe  be=new BTCe();
//			
//			HashMap<String, String> arguments=new HashMap<String, String>();
//			
//			System.out.println(be.executeQuery("getInfo", arguments).toString());
//			System.out.println(be.GetData("btc_usd", "ticker"));
//		}
//
//	}
}
