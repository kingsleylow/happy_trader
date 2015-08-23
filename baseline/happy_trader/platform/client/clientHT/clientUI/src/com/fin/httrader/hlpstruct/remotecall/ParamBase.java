/*
 * ParamBase.java
 *
 
 */
package com.fin.httrader.hlpstruct.remotecall;

import com.fin.httrader.hlpstruct.Order;
import com.fin.httrader.hlpstruct.Position;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.Uid;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 *
 * @author Victor_Zoubok
 */
public abstract class ParamBase<T> {

	static public final int PT_In = 1;
	static public final int PT_Out = 2;
	static public final int PT_InOut = 3;
	//
	private int paramType_m = 3;
	private StringBuilder paramName_m = new StringBuilder();
	private T obj_m = null;

	//
	protected ParamBase() {
	}

	public String getParamName() {
		return paramName_m.toString();
	}

	public T get() {
		return obj_m;
	}

	public void set(T value) {
		obj_m = value;
	}

	public void setParamName(String name) {
		paramName_m.setLength(0);
		paramName_m.append(name);
	}

	public int getType() {
		return paramType_m;
	}

	public void setType(int ntype) {
		paramType_m = ntype;
	}

	// to be overrided
	public abstract void convertToXmlParameter(XmlParameter xmlparameter) throws Exception;

	public abstract void convertFromXmlparameter(XmlParameter xmlparameter) throws Exception;

	// this instantiates the necessary class
	public static <T extends ParamBase> T instantiate(String instName) throws Exception {

		if (instName.equals("Bool_Proxy")) {
			ParamBase<Boolean> param = new Bool_Proxy();
			param.set(Boolean.FALSE);

			return (T) param;
		} else if (instName.equals("BoolList_Proxy")) {
			ParamBase<List<Boolean>> param = new BoolList_Proxy();
			param.set(new ArrayList<Boolean>());

			return (T) param;
		} else if (instName.equals("Int_Proxy")) {
			ParamBase<Long> param = new Int_Proxy();
			param.set(new Long(-1));

			return (T) param;
		}	else if (instName.equals("Double_Proxy")) {
			ParamBase<Double> param = new Double_Proxy();
			param.set(new Double(-1.0));

			return (T) param;
		}
		else if (instName.equals("String_Proxy")) {
			ParamBase<String> param = new String_Proxy();
			param.set(new String());

			return (T) param;
		} else if (instName.equals("StringList_Proxy")) {
			ParamBase<List<String>> param = new StringList_Proxy();
			param.set(new ArrayList<String>());

			return (T) param;
		} else if (instName.equals("IntList_Proxy")) {
			ParamBase<List<Long>> param = new IntList_Proxy();
			param.set(new ArrayList<Long>());

			return (T) param;
		} else if (instName.equals("Uid_Proxy")) {
			ParamBase<Uid> param = new Uid_Proxy();
			param.set(new Uid());

			return (T) param;
		} else if (instName.equals("UidList_Proxy")) {
			ParamBase<List<Uid>> param = new UidList_Proxy();
			param.set(new ArrayList<Uid>());

			return (T) param;
		} else if (instName.equals("PositionList_Proxy")) {
			ParamBase<List<Position>> param = new PositionList_Proxy();
			param.set(new ArrayList<Position>());

			return (T) param;
		} else if (instName.equals("Order_Proxy")) {
			ParamBase<Order> param = new Order_Proxy();
			param.set(new Order());

			return (T) param;
		} else if (instName.equals("RtData_Proxy")) {
			ParamBase<HtRtData> param = new RtData_Proxy();
			param.set(new HtRtData());

			return (T) param;
		} else if (instName.equals("RtDataList_Proxy")) {
			ParamBase<List<HtRtData>> param = new RtDataList_Proxy();
			param.set(new ArrayList<HtRtData>());

			return (T) param;
		}





		throw new HtException("ParamBase", "instantiate", "Invalid type name: \"" + instName + "\"");

	}
}
