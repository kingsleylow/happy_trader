/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.webserver.ajaxutils;

import com.fin.httrader.utils.HtBase64;
import com.fin.httrader.utils.HtStringByteConverter;
import java.io.IOException;
import javax.servlet.http.HttpServletResponse;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;

/**
 *
 * @author Victor_Zoubok
 */
public class AjaxResponse {
		public static final int CODE_OK = 0;
    public static final int CODE_FAIL = -1;

    private String reason_m = "";
    private int code_m = CODE_OK;
    private String customData_m = "";

    public AjaxResponse(int code, String reason) {
        setError(code, reason);
    }

    public AjaxResponse() {
    }

    public int getCode() {
        return code_m;
    }

    public String getReason() {
        return reason_m;
    }

    public String getCustomData() {
        return customData_m;
    }

    public void setCustomData(String data) {
        customData_m = (data != null ? data : "");
    }

    public final void setError(int code, String reason) {
        code_m = code;
        reason_m = (reason != null ? reason : "");
    }

    public String createJson() {
        JSONObject obj = new JSONObject();
        obj.put("code", new Integer(code_m));
        obj.put("reason", reason_m);

        // must base 64 encode here

        byte[] customData = HtStringByteConverter.encodeUTF8(customData_m);
        if (customData != null) {
            String customDataEncoded = HtBase64.encode(customData);
            obj.put("custom_data", customDataEncoded);
        } else {
            obj.put("custom_data", "");
        }

        return JSONValue.toJSONString(obj);
    }

    public String serializeToXmlString() {
        StringBuilder out = new StringBuilder();
        out.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        out.append("<ajax_call>");

        out.append("<status_code>");
        out.append(getCode());
        out.append("</status_code>");

        out.append("<status_string>");
        out.append(HtBase64.encode(HtStringByteConverter.encodeUTF8(getReason())));
        out.append("</status_string>");

        out.append("<data_string>");
        out.append(HtBase64.encode(HtStringByteConverter.encodeUTF8(getCustomData())));
        out.append("</data_string>");

        out.append("</ajax_call>");


        return out.toString();
    }

    public void serializeToXml(HttpServletResponse res) throws IOException {

        res.setCharacterEncoding(HtStringByteConverter.UTF8_CHARSET.displayName());
        res.getOutputStream().print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
        res.getOutputStream().print("<ajax_call>");

        res.getOutputStream().print("<status_code>");
        res.getOutputStream().print(getCode());
        res.getOutputStream().print("</status_code>");

        res.getOutputStream().print("<status_string>");

        // we are going to hexlify all these bytes
        res.getOutputStream().print(HtBase64.encode(HtStringByteConverter.encodeUTF8(getReason())));
        res.getOutputStream().print("</status_string>");

        res.getOutputStream().print("<data_string>");
        res.getOutputStream().print(HtBase64.encode(HtStringByteConverter.encodeUTF8(getCustomData())));
        res.getOutputStream().print("</data_string>");

        res.getOutputStream().print("</ajax_call>");

    }

}
