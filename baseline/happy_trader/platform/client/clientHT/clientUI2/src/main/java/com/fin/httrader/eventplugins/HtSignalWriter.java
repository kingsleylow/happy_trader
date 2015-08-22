/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins;

import com.fin.httrader.interfaces.HtEventPlugin;
import com.fin.httrader.utils.HtDbUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtFileUtils;
import com.fin.httrader.utils.HtUtils;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import com.mysql.jdbc.Connection;


import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.HashSet;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class HtSignalWriter implements HtEventPlugin {

	private HashSet<String> modules_m = new HashSet<String>();
	private String name_m = "";
	private String dbHost_m = "";
	private int dbPort_m = -1;
	private String dbUser_m = "";
	private String dbPassword_m = "";
	private String dbDatabase_m = "";
	com.mysql.jdbc.Connection conn_m = null;

	// -----------------
	private String getContext() {
		return HtSignalWriter.class.getSimpleName();
	}

	@Override
	public Integer[] returnSubscribtionEventTypes() {
		return new Integer[]{XmlEvent.ET_DrawableObject};
	}

	@Override
	public void initialize(Map<String, String> initdata) throws Exception {
		String symbolFile = HtUtils.getParameterWithException(initdata, "SYMBOL_FILE");
		modules_m.addAll(HtFileUtils.readSymbolListFromFile(symbolFile));

		dbHost_m = HtUtils.getParameterWithException(initdata, "DB_HOST");
		dbPort_m = HtUtils.parseInt(HtUtils.getParameterWithException(initdata, "DB_PORT"));
		dbUser_m = HtUtils.getParameterWithException(initdata, "DB_USER");
		dbPassword_m = HtUtils.getParameterWithException(initdata, "DB_PASSWORD");
		dbDatabase_m = HtUtils.getParameterWithException(initdata, "DB_DATABASE");

		// need to check connections
		String url = "jdbc:mysql://" + dbHost_m + ":" + dbPort_m + "/" + dbDatabase_m;


		Class.forName("com.mysql.jdbc.Driver").newInstance();
		conn_m = (Connection) DriverManager.getConnection(url, dbUser_m, dbPassword_m);


	}

	@Override
	public void deinitialize() throws Exception {
		HtDbUtils.silentlyCloseConnection(conn_m);
	}

	@Override
	public String getEventPluginId() {
		return name_m;
	}

	@Override
	public void setEventPluginId(String pluginId) {
		name_m = pluginId;
	}

	@Override
	public void execute(XmlEvent alertData) throws Exception {
		HtDrawableObject drobj = alertData.getAsDrawableObject();
		if (modules_m.contains(drobj.getName())) {

			String sql = "insert into signals(session_id, module, time, data, provider, symbol) values (?,?,?,?,?,?)";
			PreparedStatement stmt = null;

			try {

				//StringBuilder data = new StringBuilder();
				XmlParameter param = new XmlParameter();
				HtDrawableObject.drawableObjectToParameter(drobj, param);
				
				
				stmt = conn_m.prepareStatement(sql);
				stmt.setString(1, drobj.getRunName());
				stmt.setString(2, drobj.getName());
				stmt.setDouble(3, drobj.getTime());
				stmt.setString(4, XmlHandler.serializeParameterStatic(param));
				stmt.setString(5, drobj.getProvider());
				stmt.setString(6, drobj.getSymbol());
				
				stmt.execute();
				
				
				
				/*
				if (drobj.getType() == HtDrawableObject.DO_TEXT) {
					HtDrawableObject.Text txtobj = drobj.getAsText();
					data.append(txtobj.text_m.toString());
				} else if (drobj.getType() == HtDrawableObject.DO_LINE_INDIC) {
					List<HtDrawableObject.LineIndicator> indicobj = drobj.getAsLineIndicator();

					for(int i = 0; i < indicobj.size(); i++) {
						HtDrawableObject.LineIndicator li = indicobj.get(i);
						
						
					}

				}
				 * 
				 */
				
			} catch (SQLException sqe) 
			{
				throw new HtException(getContext(), "execute", "SQL exception: " + sqe.getMessage());
			} catch (Throwable e)
			{
				throw new HtException(getContext(), "execute", "Exception: " + e.getMessage());
			} finally 
			{
				HtDbUtils.silentlyCloseStatement(stmt);
			}
		}

	}
}
