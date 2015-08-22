


// =======================================================
// Testcase MT4 Proxy



//
TESTCASE_BEGIN( test3 )

	// =======================================================
	// Setup
	

	TESTCASE_SETUP
	{
		
		
	}


	// =======================================================
	// Wrapup
	TESTCASE_WRAPUP
	{
		
	}


	// =======================================================
	// Test


	// test checking communication with mtproxy_bck
	DEFINE_TEST( T1 ) 
	{
		CppUtils::StringList tmpLst;
		
		BrkLib::String_Proxy strproxy;
		
		CppUtils::String val = CppUtils::String("25");
		CppUtils::String const& dummy_str = val;
		strproxy.get() = dummy_str;
		val = strproxy.get();
		
		BrkLib::Order order;
		
		cout << "Order" << endl;
		cout << order.toString() << endl;

		cout << "BrokerResponseBase" << endl;
		BrkLib::BrokerResponseBase base;
		cout << base.toString() << endl;



		cout << "BrokerResponseOrder" << endl;
		BrkLib::BrokerResponseOrder bresporder;
		BrkLib::BrokerResponseBase* border = &bresporder;
		cout << border->toString() << endl;

		



	} 

	// Broker communication
	DEFINE_TEST( T2 ) 
	{
		// create XML
		BrkLib::Order order;
		order.ID_m.generate();
		order.symbolNum_m = "EUR";
		order.symbolDenom_m = "USD";
		order.orVolume_m = 100000;
		order.orderType_m = BrkLib::OP_BUY;
		order.orPrice_m = 1.3456;
		order.orSLprice_m = 1.3400;
		order.orTPprice_m = 1.3500;

		// create close order
		BrkLib::Order order_close;
		order_close.ID_m.generate();
		order_close.symbolNum_m = "EUR";
		order_close.symbolDenom_m = "USD";
		order_close.brokerPositionID_m = "(to change)";
		order_close.orderType_m = BrkLib::OP_CLOSE;



		HlpStruct::CallingContext context;
		context.getCallName() = "createSessionObject";
		BrkLib::String_Proxy connect_proxy;
		connect_proxy.get() = "dummy";

		BrkLib::BrokerSession_Proxy session_proxy;

		context.setParameter("session", session_proxy );
		context.setParameter("connectstring", connect_proxy);
		// generate UID
		session_proxy.get().generate();

		HlpStruct::XmlParameter parameter_sess;
		HlpStruct::convertCallingContextToXmlParameter(context, parameter_sess);
		
		CppUtils::String strparameter;
		serializeXmlParameter(parameter_sess, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_session.xml"), strparameter);

		HlpStruct::XmlParameter parameter_connect;
		// command to create connection
		HlpStruct::CallingContext context2;
		context2.getCallName() = "connectToBroker";
		context2.setParameter("session", session_proxy);
		HlpStruct::convertCallingContextToXmlParameter(context2, parameter_connect);
		serializeXmlParameter(parameter_connect, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_connect.xml"), strparameter);

		HlpStruct::XmlParameter parameter_order;
		// command to open order
		HlpStruct::CallingContext context3;
		context3.getCallName() = "unblockingIssueOrder";
		context3.setParameter("session", session_proxy);
		BrkLib::Bool_Proxy result;
		context3.setParameter("result", result);

		BrkLib::Order_Proxy order_proxy;
		order_proxy.get() = order;
		context3.setParameter("order", order_proxy);

		HlpStruct::convertCallingContextToXmlParameter(context3, parameter_order);
		serializeXmlParameter(parameter_order, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_order.xml"), strparameter);

		// command to close order
		HlpStruct::XmlParameter parameter_order_close;
		HlpStruct::CallingContext context4;
		context4.getCallName() = "unblockingIssueOrder";
		context4.setParameter("session", session_proxy);
		BrkLib::Bool_Proxy result2;
		context4.setParameter("result", result2);

		BrkLib::Order_Proxy order_proxy_close;
		order_proxy_close.get() = order_close;
		context4.setParameter("order", order_proxy_close);

		HlpStruct::convertCallingContextToXmlParameter(context4, parameter_order_close);
		serializeXmlParameter(parameter_order_close, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_order_close.xml"), strparameter);

		//

		// xml command template
		HlpStruct::XmlParameter command_tmpl0;
		command_tmpl0.getParameter("proxy_name").getAsString(true) = "server_proxy";
		command_tmpl0.getName() = "push_thread_event";
		command_tmpl0.getParameter("server_name").getAsString(true) = "Server 1";
		command_tmpl0.getParameter("algorithm_thread_id").getAsInt(true) = -1;
		
		// create event structure
		HlpStruct::EventData event0;
		event0.setEventType(HlpStruct::EventData::ET_AlgorithmEvent);
		event0.setType(HlpStruct::EventData::DT_CommonXml);
		event0.getAsXmlParameter() = parameter_sess;

		HlpStruct::XmlParameter total_event0;
		HlpStruct::convertEventToXmlParameter(event0, total_event0);

		command_tmpl0.getXmlParameter("event") = total_event0;
		
		//
		
		// xml command template
		HlpStruct::XmlParameter command_tmpl1;
		command_tmpl1.getParameter("proxy_name").getAsString(true) = "server_proxy";
		command_tmpl1.getName() = "push_thread_event";
		command_tmpl1.getParameter("server_name").getAsString(true) = "Server 1";
		command_tmpl1.getParameter("algorithm_thread_id").getAsInt(true) = -1;
		
		// create event structure
		HlpStruct::EventData event1;
		event1.setEventType(HlpStruct::EventData::ET_BrokerEvent);
		event1.setType(HlpStruct::EventData::DT_CommonXml);
		event1.getAsXmlParameter() = parameter_connect;

		HlpStruct::XmlParameter total_event1;
		HlpStruct::convertEventToXmlParameter(event1, total_event1);

		command_tmpl1.getXmlParameter("event") = total_event1;


		//
	
		// xml command template
		HlpStruct::XmlParameter command_tmpl;
		command_tmpl.getParameter("proxy_name").getAsString(true) = "server_proxy";
		command_tmpl.getName() = "push_thread_event";
		command_tmpl.getParameter("server_name").getAsString(true) = "Server 1";
		command_tmpl.getParameter("algorithm_thread_id").getAsInt(true) = -1;
		
		// create event structure
		HlpStruct::EventData event;
		event.setEventType(HlpStruct::EventData::ET_BrokerEvent);
		event.setType(HlpStruct::EventData::DT_CommonXml);
		event.getAsXmlParameter() = parameter_order;

		HlpStruct::XmlParameter total_event;
		HlpStruct::convertEventToXmlParameter(event, total_event);

		command_tmpl.getXmlParameter("event") = total_event;

		// event to close 
		HlpStruct::XmlParameter command_tmpl25;
		command_tmpl25.getParameter("proxy_name").getAsString(true) = "server_proxy";
		command_tmpl25.getName() = "push_thread_event";
		command_tmpl25.getParameter("server_name").getAsString(true) = "Server 1";
		command_tmpl25.getParameter("algorithm_thread_id").getAsInt(true) = -1;

		//
		HlpStruct::EventData event25;
		event25.setEventType(HlpStruct::EventData::ET_BrokerEvent);
		event25.setType(HlpStruct::EventData::DT_CommonXml);
		event25.getAsXmlParameter() = parameter_order_close;

		HlpStruct::XmlParameter total_event125;
		HlpStruct::convertEventToXmlParameter(event25, total_event125);

		command_tmpl25.getXmlParameter("event") = total_event125;


		// wrap globals
		HlpStruct::XmlParameter global_cmd;
		global_cmd.getName() = "broker_test";
		global_cmd.getXmlParameterList("server_commands").push_back(command_tmpl0);
		global_cmd.getXmlParameterList("server_commands").push_back(command_tmpl1);
		global_cmd.getXmlParameterList("server_commands").push_back(command_tmpl);
		serializeXmlParameter(global_cmd, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_template.xml"), strparameter);

		// close order command
		HlpStruct::XmlParameter global_cmd3;
		global_cmd3.getName() = "broker_test";
		global_cmd3.getXmlParameterList("server_commands").push_back(command_tmpl0);
		global_cmd3.getXmlParameterList("server_commands").push_back(command_tmpl1);
		global_cmd3.getXmlParameterList("server_commands").push_back(command_tmpl25);
		serializeXmlParameter(global_cmd3, strparameter);
		CppUtils::saveContentInFile2(CppUtils::String("c:\\temp\\command_template_order_close.xml"), strparameter);


		 
		
	}
		
	// =======================================================
	// Test Index

	TESTCASE_INDEX( test3 )
	{
		REGISTER_TEST( test3, T1 );
		REGISTER_TEST( test3, T2 );
	}


TESTCASE_END;
