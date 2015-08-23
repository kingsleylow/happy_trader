#ifndef _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_MT_RESPONSE_INCLUDED
#define _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_MT_RESPONSE_INCLUDED

#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"


#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"

/**
* This file encapsulate JSON exchange packets, so keep in hpp file to use in mt proxy project
*/
namespace AlgLib {

		class AlgLib_Event_Dialog_Response {
		public:
			
			static BaseDlgEntity* creator(json::Object& json_init_root) {
				CppUtils::String command = CppUtils::toUpperCase((json::String const&)json_init_root["COMMAND_NAME"]);
			
				BEGIN_CREATOR_DLG_ENTRY(command.c_str());

					CREATOR_DLG_ENTRY(Resp_G_GET_ALL_METADATA);
					CREATOR_DLG_ENTRY(Resp_G_QUERY_POSITIONS_FOR_ACCOUNT);
					CREATOR_DLG_ENTRY(Resp_G_QUERY_AUTOLOAD_USERS);
					CREATOR_DLG_ENTRY(Resp_START_POLLING_USERS);
					CREATOR_DLG_ENTRY(Resp_REQUEST_POLLING_USER_LIST);
					CREATOR_DLG_ENTRY(Resp_REQUEST_MT_STATUS);
					CREATOR_DLG_ENTRY(Resp_UPDATE_USER_ACCESS_TIME);
					CREATOR_DLG_ENTRY(Resp_CLEAR_ALL_USERS);
					CREATOR_DLG_ENTRY(Resp_G_KILL_MT_PROCESS);
					CREATOR_DLG_ENTRY(Resp_START_ORDER_LIST);
					

				END_CREATOR_DLG_ENTRY()


				return NULL;
		}

		
		class AlgLibResponseBase : public BaseDlgEntity {
				public:

					AlgLibResponseBase():
						result_code(0),
						mt_result_code(0),
						mt_result_message("OK"),
						result_message("OK")
					{
					};

					// some common serialization work
					void serializeBase(json::Object& json_init_root) const
					{
						// command name
						json_init_root["COMMAND_NAME"]  =json::String(getCommandName());
						

						json_init_root["RESULT_MESSAGE"] = json::String(result_message);
						json_init_root["RESULT_CODE"] = json::Number(result_code);
						json_init_root["MT_MESSAGE"] = json::String(mt_result_message);
						json_init_root["MT_CODE"] = json::Number(mt_result_code);
						json_init_root["MT_INSTANCE_NAME"] = json::String(mt_instance_name_m);
						//json_init_root["MT_INSTANCE_UID"] = json::String(mt_instance_uid_m.toString());
					}

					void deserializeBase(json::Object const& json_init_root)
					{
						mt_instance_name_m = (json::String const&)json_init_root["MT_INSTANCE_NAME"];
						mt_result_code = (json::Number const&)(json_init_root["RESULT_CODE"]);
						mt_result_message = (json::String const&)json_init_root["MT_MESSAGE"];
						result_message = (json::String const&)json_init_root["RESULT_MESSAGE"];
						result_code =  (json::Number const&)json_init_root["RESULT_CODE"];
						//mt_instance_uid_m.fromString( (json::String const&)json_init_root["MT_INSTANCE_UID"] );

						// command name is initialized automatically
					}

					// also it has default toJson and fromJson implementations
					// to avoid re-write code in deraived
					virtual json::Object toJson() const
					{
							json::Object json_init_root;
							serializeBase(json_init_root);
							
							// need proceed upper level
							return json_init_root;
					};

					virtual void fromJson(json::Object const& json_init_root)
					{
							deserializeBase(json_init_root);
					}

					// base members

					CppUtils::String mt_instance_name_m;
					//CppUtils::Uid mt_instance_uid_m;
					CppUtils::String result_message;
					CppUtils::String mt_result_message;
					int result_code;
					int mt_result_code;

			};

			class Resp_Dummy: public AlgLibResponseBase
			{
				public:

					MCTOR(Resp_Dummy, "DUMMY");

			};

			// -------------------------------

			class Resp_G_GET_ALL_METADATA: public AlgLibResponseBase
			{
				public:

					MCTOR(Resp_G_GET_ALL_METADATA, "G_GET_ALL_METADATA")
					

					struct MtEntry {
						bool connected_m;
						CppUtils::String name_m;
						long lastHeartBeat_m;
						bool loadBalanced_m;
						CppUtils::Uid terminalUid_m;
						int pendingSize_m;
						long lastRestartTime_m;
						int restartCount_m;
						int pid_m;
						CppUtils::String companyId_m;
					};

					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						
						json::Array mt_instances_arr_json;
						json::Array ev_plug_params_arr_json;

						serializeBase(json_init_root);

						for(auto it = eventPluginParameters_m.begin(); it != eventPluginParameters_m.end(); it++) {
							CppUtils::String const & value = it->second;
							CppUtils::String const & name = it->first;

							if (name.size() > 0) {
								json::Object ev_plug_entry;
								ev_plug_entry[name] = (json::String)value;

								ev_plug_params_arr_json.Insert(ev_plug_entry);
							}
							
						}

						for(int i = 0; i < data_m.size(); i++) {
							MtEntry const& d = data_m[ i ];

							json::Object mt_instance;
						
							mt_instance["CONNECTED"] = json::Boolean(d.terminalUid_m.isValid());
							mt_instance["NAME"] = json::String(d.name_m);
							mt_instance["LAST_HEARTBEAT"] = json::Number(d.lastHeartBeat_m);
							mt_instance["LOAD_BALANCED"] = json::Boolean(d.loadBalanced_m);
							mt_instance["TERMINAL_UID"] = json::String(d.terminalUid_m.toString());
							mt_instance["PENDING_SIZE"] = json::Number(d.pendingSize_m);
							mt_instance["LAST_RESTART_TIME"] = json::Number(d.lastRestartTime_m);
							mt_instance["RESTART_COUNT"] = json::Number(d.restartCount_m);
							mt_instance["PID"] = json::Number(d.pid_m);
							mt_instance["COMPANY_ID"] = json::String(d.companyId_m);


							mt_instances_arr_json.Insert( mt_instance );
						}

						json::Object data_json;
						
						data_json["MT_INSTANCES"] = mt_instances_arr_json;
						data_json["EVENT_PLUGIN_PARAMETERS"] = ev_plug_params_arr_json;

						json_init_root["DATA"] = data_json;

						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							deserializeBase(json_init_root);
							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& mt_instances_arr_json  = data_json["MT_INSTANCES"];
							json::Array const& ev_plug_params_arr_json  = data_json["EVENT_PLUGIN_PARAMETERS"];

							data_m.clear();
							eventPluginParameters_m.clear();

							for(int i = 0; i < ev_plug_params_arr_json.Size(); i++) {
								json::Object const& obj_i = ev_plug_params_arr_json[i];
								CppUtils::String const& name = (json::String const&)obj_i["NAME"];
								if (name.size() <=0)
									continue;

								eventPluginParameters_m[name] = (json::String const&)obj_i["VALUE"];
							}

							for(int i = 0; i < mt_instances_arr_json.Size(); i++) {
								json::Object const& obj_i = mt_instances_arr_json[i];

								MtEntry entry;

								entry.connected_m =  (json::Boolean const&)obj_i["CONNECTED"];
								entry.name_m =  (json::String const&)obj_i["NAME"];
								entry.lastHeartBeat_m  = (json::Number const&)obj_i["LAST_HEARTBEAT"];
								entry.loadBalanced_m = (json::Boolean const&)obj_i["LOAD_BALANCED"];
								entry.terminalUid_m.fromString((json::String const&)obj_i["TERMINAL_UID"]);
								entry.pendingSize_m = (json::Number const&)obj_i["PENDING_SIZE"];

								entry.lastRestartTime_m =   (json::Number const&)obj_i["LAST_RESTART_TIME"];
								entry.restartCount_m = (json::Number const&)obj_i["RESTART_COUNT"];
								entry.pid_m = (json::Number const&)obj_i["PID"];
								entry.companyId_m = (json::String const&)obj_i["COMPANY_ID"];

								data_m.push_back( entry );
							}
						
					}

					// members
					vector<MtEntry> data_m;
					CppUtils::StringMap eventPluginParameters_m;

			};


			// ------------------------------

			class Resp_CLEAR_ALL_USERS: public AlgLibResponseBase
			{
				public:
				MCTOR(Resp_CLEAR_ALL_USERS, "CLEAR_ALL_USERS")

			};

			// ------------------------------

			class Resp_G_QUERY_AUTOLOAD_USERS: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_G_QUERY_AUTOLOAD_USERS, "G_QUERY_AUTOLOAD_USERS");

					virtual json::Object toJson() const
					{
				
							json::Object json_init_root;
							serializeBase(json_init_root);

							
							json::Array mt_list_arr_json;

							for(auto const& d : instances_m){
						
								json::Object mt_instance;

								mt_instance["NAME"] = json::String(d.name_m);
								mt_instance["COMPANY_ID"] = json::String(d.companyId_m);
								mt_instance["INSTANCE_UID"] = json::String(d.isntanceUid_m.toString());

								json::Array autoload_user_list_arr_json;
								

								for(auto const& u : d.autoLoadUserList_m){
									json::Object user_i;

									user_i["NAME"] = json::String(u.username_m);
									user_i["PASSWORD"] = json::String(u.password_m);
									user_i["LIFETIME"] = json::Number(u.lifeTime_m);
									user_i["SERVER"] = json::String(u.server_m);

									autoload_user_list_arr_json.Insert( user_i );
								}
								


								mt_instance["AUTOLOAD_USER_LIST"] = autoload_user_list_arr_json;
								


								mt_list_arr_json.Insert(mt_instance);

							}

							json::Object data_json;
							data_json["MT_LIST"] = mt_list_arr_json;
							json_init_root["DATA"] = data_json;
							

							// need proceed upper level
							return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							deserializeBase(json_init_root);

							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& mt_list_arr_json  = data_json["MT_LIST"];

							instances_m.clear();

							for(int i = 0; i < mt_list_arr_json.Size(); i++) {
								json::Object const& mt_instance = mt_list_arr_json[i];

								MtInstance inst_i;
								inst_i.companyId_m = (json::String const&)mt_instance["COMPANY_ID"];
								inst_i.isntanceUid_m.fromString((json::String const&)mt_instance["INSTANCE_UID"]);
								inst_i.name_m = (json::String const&)mt_instance["NAME"];

								json::Array const& autoload_user_list_arr_json  = data_json["AUTOLOAD_USER_LIST"];
								

								for(int k = 0; k < autoload_user_list_arr_json.Size(); k++) {
									json::Object const& user_i = autoload_user_list_arr_json[i];

									MtUser d;
									d.username_m = (json::String const&)user_i["NAME"];
									d.password_m = (json::String const&)user_i["PASSWORD"];
									d.server_m = (json::String const&)user_i["SERVER"];

									json::SafeConvertor<json::Number> lt_converter( user_i["LIFETIME"] );
									d.lifeTime_m = lt_converter.getAsDouble();

									inst_i.autoLoadUserList_m.push_back(d);

								}

								

								instances_m.push_back(inst_i);
																
							}
						
					}



					// user list
					struct MtUser {
						MtUser(): 
							lifeTime_m(-1)
						{
						};

						CppUtils::String username_m;
						CppUtils::String password_m;
						CppUtils::String server_m;
						int lifeTime_m;
					};

					struct MtInstance {
						vector<MtUser> autoLoadUserList_m;

						
						// company ID
						CppUtils::String companyId_m;

						// mt instance name
						CppUtils::String name_m;

						// mt instnace UID
						CppUtils::Uid isntanceUid_m;
					};

					// main data
					vector<MtInstance> instances_m;
			};

			// ------------------------------

			class Resp_G_QUERY_POSITIONS_FOR_ACCOUNT: public AlgLibResponseBase
			{
				public:

					MCTOR(Resp_G_QUERY_POSITIONS_FOR_ACCOUNT, "G_QUERY_POSITIONS_FOR_ACCOUNT")

					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						serializeBase(json_init_root);
						
						json::Array order_list_json;

						for(int i = 0; i < posList_m.size(); i++) {
							BrkLib::Position const& pos = posList_m[ i ];

							json::Object pos_json_i;

							pos_json_i["ticket"] = json::Number(atoi(pos.brokerPositionID_m.c_str()));
							pos_json_i["symbol"] = json::String(pos.symbol_m);
							pos_json_i["open_price"] = json::String(CppUtils::float2String(pos.avrOpenPrice_m, -1,4));
							pos_json_i["close_price"] = json::String(CppUtils::float2String(pos.avrClosePrice_m, -1,4));
							pos_json_i["open_time"] = json::String(CppUtils::float2String(pos.timeOpen_m, -1,4));
							pos_json_i["close_time"] = json::String(CppUtils::float2String(pos.timeClose_m, -1,4));

							if (pos.direction_m == BrkLib::TradeDirection::TD_SHORT) {
								pos_json_i["type_code"] = json::String("SELL");
							}
							else if (pos.direction_m == BrkLib::TradeDirection::TD_LONG) {
								pos_json_i["type_code"] = json::String("BUY");
							}

							order_list_json.Insert(pos_json_i);
						}

						json::Object acc_info_object;
											
						json::Object data_object;
						data_object["HISTORY_ORDERS"]["ORDER_LIST"] = order_list_json;
						data_object["ACCOUNT_INFO"]["ACCOUNT_NUMBER"] = json::Number(atoi(accNumber_m.c_str()));

						json_init_root["DATA"] = data_object;

						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
						
						json::Object const& data_object_json = json_init_root["DATA"];

					
						
						// fill in pos list
						posList_m.clear();
						accNumber_m = (json::String const&)data_object_json["ACCOUNT_INFO"]["ACCOUNT_NUMBER"];

						json::Array const& order_list_json = data_object_json["HISTORY_ORDERS"]["ORDER_LIST"];
						for(int i = 0; i < order_list_json.Size(); i++) {
							json::Object const& obj_i = order_list_json[i];

							BrkLib::Position pos;
							//
							pos.brokerPositionID_m = CppUtils::long2String((json::Number const&)obj_i["ticket"]);
							pos.symbol_m = (json::String const&) obj_i["symbol"];
							pos.avrOpenPrice_m =  CppUtils::string2Float( (json::String const&)obj_i["open_price"]);
							pos.avrClosePrice_m = CppUtils::string2Float( (json::String const&)obj_i["close_price"]); 
							pos.timeOpen_m =    CppUtils::string2Float(   (json::String const&)obj_i["open_time"]);
							pos.timeClose_m =   CppUtils::string2Float(   (json::String const&)obj_i["close_time"] );

							if (CppUtils::toUpperCase((json::String const&)obj_i["type_code"]) == "SELL") {
								pos.direction_m = BrkLib::TradeDirection::TD_SHORT;
							}
							else if (CppUtils::toUpperCase((json::String const&)obj_i["type_code"]) == "BUY") {
								pos.direction_m = BrkLib::TradeDirection::TD_LONG;
							}

							posList_m.push_back( pos );
						
						}
						
					}

					// members
					BrkLib::PositionList posList_m;
					CppUtils::String accNumber_m;

			};


			// --------------------------------

			class Resp_START_POLLING_USERS: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_START_POLLING_USERS, "START_POLLING_USERS");

			};

			// ---------------------------------

			class Resp_G_KILL_MT_PROCESS: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_G_KILL_MT_PROCESS, "G_KILL_MT_PROCESS");

					virtual json::Object toJson() const
					{
				
							json::Object json_init_root;
							serializeBase(json_init_root);

							json_init_root["IS_KILLED"] = json::Boolean(isKilled_m);

							// need proceed upper level
							return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							deserializeBase(json_init_root);

							isKilled_m = (json::Boolean const&)json_init_root["IS_KILLED"];
					}

					bool isKilled_m;

			};

			

			// ---------------------------------

			class Resp_REQUEST_POLLING_USER_LIST: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_REQUEST_POLLING_USER_LIST, "REQUEST_POLLING_USER_LIST");

					virtual json::Object toJson() const
					{
				
							json::Object json_init_root;
							serializeBase(json_init_root);

							json::Array user_lsit_arr_json;
						

							for(auto const& d : userList_m){
						
								json::Object mt_instance;
						
								mt_instance["NAME"] = json::String(d.UserName_m);
								mt_instance["PASSWORD"] = json::String(d.password_m);
								mt_instance["LIFETIME"] = json::Number(d.lifeTime_m);
								mt_instance["SERVER"] = json::String(d.server_m);
					

								user_lsit_arr_json.Insert( mt_instance );
							}

							json::Object data_json;
							data_json["USER_LIST"] = user_lsit_arr_json;
							json_init_root["DATA"] = data_json;




							// need proceed upper level
							return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							deserializeBase(json_init_root);

							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& user_arr_json  = data_json["USER_LIST"];

							userList_m.clear();

							for(int i = 0; i < user_arr_json.Size(); i++) {
								json::Object const& obj_i = user_arr_json[i];

								MtUser d;
								d.UserName_m = (json::String const&)obj_i["NAME"];
								d.password_m = (json::String const&)obj_i["PASSWORD"];
								d.server_m = (json::String const&)obj_i["SERVER"];

								json::SafeConvertor<json::Number> lt_converter( obj_i["LIFETIME"] );
								d.lifeTime_m = lt_converter.getAsDouble();

								userList_m.push_back( d );
							}
						
					}

					// user list
					struct MtUser {
						MtUser(): 
							lifeTime_m(-1)
						{
						};

						CppUtils::String UserName_m;
						CppUtils::String password_m;
						CppUtils::String server_m;
						int lifeTime_m;
					};

					vector<MtUser> userList_m;


			};

			// --------------------------------

			class Resp_REQUEST_MT_STATUS: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_REQUEST_MT_STATUS, "REQUEST_MT_STATUS");

					virtual json::Object toJson() const
					{
				
							json::Object json_init_root;
							serializeBase(json_init_root);

							json_init_root["DATA"]["MT_STATUS"]["TOTAL_QUEUE_SIZE"] = json::Number(totalQueueSize_m);

							// need proceed upper level
							return json_init_root;
					};

					virtual void fromJson(json::Object const& json_init_root)
					{
							deserializeBase(json_init_root);

						
							totalQueueSize_m = (json::Number const& )json_init_root["DATA"]["MT_STATUS"]["TOTAL_QUEUE_SIZE"];
						
					}


					int totalQueueSize_m;
			};


			// --------------------------------

			class Resp_START_ORDER_LIST: public AlgLibResponseBase {
				public:
					MCTOR(Resp_START_ORDER_LIST, "START_ORDER_LIST");
			};

			// --------------------------------

			class Resp_UPDATE_USER_ACCESS_TIME: public AlgLibResponseBase
			{
				public:
					MCTOR(Resp_UPDATE_USER_ACCESS_TIME, "UPDATE_USER_ACCESS_TIME");

			};
	
	}; // end of class


}; // end of ns

#endif