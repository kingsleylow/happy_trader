#ifndef _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_MT_REQUEST_INCLUDED
#define _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_MT_REQUEST_INCLUDED

#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"
#include "../brklib/brklib.hpp"

/**
* This file encapsulate JSON exchange packets, so keep in hpp file to use in mt proxy project
*/
namespace AlgLib {



	class AlgLib_Event_Dialog_Request {
	public:

		static BaseDlgEntity* creator( CppUtils::String const& command) {
			return creatorHelper(CppUtils::toUpperCase(command));
		}

		static BaseDlgEntity* creator(json::Object& json_init_root) {
				CppUtils::String command = CppUtils::toUpperCase((json::String const&)json_init_root["MTCOMMAND_NAME"]);
				return creatorHelper(command);
		}

		static BaseDlgEntity* creatorHelper(CppUtils::String const& command)
		{
				BEGIN_CREATOR_DLG_ENTRY(command.c_str());

					CREATOR_DLG_ENTRY(Req_G_GET_ALL_METADATA);
					CREATOR_DLG_ENTRY(Req_G_QUERY_POSITIONS_FOR_ACCOUNT);
					CREATOR_DLG_ENTRY(Req_G_QUERY_AUTOLOAD_USERS);
					CREATOR_DLG_ENTRY(Req_CLEAR_ALL_USERS);
					CREATOR_DLG_ENTRY(Req_START_POLLING_USERS);
					CREATOR_DLG_ENTRY(Req_REQUEST_POLLING_USER_LIST);
					CREATOR_DLG_ENTRY(Req_REQUEST_MT_STATUS);
					CREATOR_DLG_ENTRY(Req_UPDATE_USER_ACCESS_TIME);
					CREATOR_DLG_ENTRY(Req_G_KILL_MT_PROCESS);
					CREATOR_DLG_ENTRY(Req_START_ORDER_LIST);
				
				END_CREATOR_DLG_ENTRY()
				return NULL;
		};

	
		class AlgLibRequestBase : public BaseDlgEntity{
				public:
					virtual ~AlgLibRequestBase()
					{
					}

					virtual bool routeToMt() const = 0;

			};

			// what is routed has pipe name
		

			class AlgLibRequestRouteMtBase : public AlgLibRequestBase {
				public:
					virtual bool routeToMt() const{
						return true;
					}

					CppUtils::String const& getMTInstaceName() const
					{
						return mtRecepient_m;
					}

					CppUtils::String & getMTInstaceName() 
					{
						return mtRecepient_m;
					}

					void setMTInstaceName(CppUtils::String const& mtInstnace)
					{
						mtRecepient_m = mtInstnace;
					}

					// some common serialization work
					void serializeBase(json::Object& json_init_root) const
					{
						// command name
						json_init_root["MTCOMMAND_NAME"]  =json::String(getCommandName());
						json_init_root["MT_INSTANCE"]  =json::String(mtRecepient_m);
					
					}

					void deserializeBase(json::Object const& json_init_root)
					{
						mtRecepient_m = (json::String const&)json_init_root["MT_INSTANCE"];
					}

					// need to give toJson and from Json default implementatiosn
					virtual json::Object toJson() const
					{
						json::Object json_init_root;
						AlgLibRequestRouteMtBase::serializeBase(json_init_root);
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestRouteMtBase::deserializeBase(json_init_root);
					}
					
			private:

				  CppUtils::String mtRecepient_m;
				
			};

			
			// ------------------------------------------
		
			class AlgLibRequestNonRouteMtBase : public AlgLibRequestBase {
				public:
					virtual bool routeToMt() const{
						return false;
					}

					// some common serialization work
					void serializeBase(json::Object& json_init_root) const
					{
						// command name
						json_init_root["MTCOMMAND_NAME"]  =json::String(getCommandName());
						json_init_root["MT_INSTANCE"]  = json::String();
					
					}

					void  deserializeBase(json::Object const& json_init_root)
					{
					}

					// need to give toJson and from Json default implementatiosn
					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						AlgLibRequestNonRouteMtBase::serializeBase(json_init_root);
						
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestNonRouteMtBase::deserializeBase(json_init_root);
					}
				
			};

			// ---------------------------------------------------------

			// this gets only autoload users for all instances
			class Req_G_QUERY_AUTOLOAD_USERS: public AlgLibRequestNonRouteMtBase
			{
				public:
					MCTOR(Req_G_QUERY_AUTOLOAD_USERS, "G_QUERY_AUTOLOAD_USERS");
			};

			// ---------------------------------------------------------

			class Req_G_GET_ALL_METADATA: public AlgLibRequestNonRouteMtBase
			{
				public:
					MCTOR(Req_G_GET_ALL_METADATA, "G_GET_ALL_METADATA");

					

			};

			
			// ------------------------------------------

			class Req_G_QUERY_POSITIONS_FOR_ACCOUNT: public AlgLibRequestNonRouteMtBase
			{
				public:
					MCTOR(Req_G_QUERY_POSITIONS_FOR_ACCOUNT, "G_QUERY_POSITIONS_FOR_ACCOUNT");

					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						
						json::Array mt_instances_arr_json;
						AlgLibRequestNonRouteMtBase::serializeBase(json_init_root);

						json_init_root["DATA"]["ACCOUNT_NUMBER"] = json::String(accNumber_m);
						
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestNonRouteMtBase::deserializeBase(json_init_root);

							accNumber_m = (json::String const&)json_init_root["DATA"]["ACCOUNT_NUMBER"];
					}

					CppUtils::String accNumber_m;
			};

			// ------------------------------------------
			class Req_START_ORDER_LIST: public AlgLibRequestRouteMtBase {

				public:
					MCTOR(Req_START_ORDER_LIST, "START_ORDER_LIST");


					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						serializeBase(json_init_root);

						json::Array data_list_arr_json;
						

						for(auto const& order : orders_m){
						
							json::Object mt_instance;
							mt_instance["ORDER_DATA"] = json::String(order);
							data_list_arr_json.Insert( mt_instance );
						}

						json::Object data_json;
						data_json["ORDER_LIST"] = data_list_arr_json;
						json_init_root["DATA"] = data_json;
						
						
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							deserializeBase(json_init_root);


							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& data_list_arr_json  = data_json["ORDER_LIST"];

							orders_m.clear();

							for(int i = 0; i < data_list_arr_json.Size(); i++) {
								json::Object const& obj_i = data_list_arr_json[i];

								CppUtils::String const& data_i = (json::String const&)obj_i["ORDER_DATA"];
								orders_m.push_back( data_i );
							}
							
					}


				// so far textual presentation
				CppUtils::StringList orders_m;

			};
			
			// ------------------------------------------

			class Req_CLEAR_ALL_USERS: public AlgLibRequestRouteMtBase
			{
				public:
					MCTOR(Req_CLEAR_ALL_USERS, "CLEAR_ALL_USERS");
					

			};

			
			// ------------------------------------------

			class Req_REQUEST_POLLING_USER_LIST: public AlgLibRequestRouteMtBase {
				public:
					MCTOR(Req_REQUEST_POLLING_USER_LIST, "REQUEST_POLLING_USER_LIST");
					

			
			};

			// --------------------

			// note that mt instnace here is optional parameter
			// though at the end if must be resolved
			
			class Req_START_POLLING_USERS: public AlgLibRequestRouteMtBase
			{
				public:
					MCTOR(Req_START_POLLING_USERS, "START_POLLING_USERS");
					
					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						
						AlgLibRequestRouteMtBase::serializeBase(json_init_root);

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
						json_init_root["COMPANY_NAME"] = json::String(companyName_m);



						
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestRouteMtBase::deserializeBase(json_init_root);

							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& user_arr_json  = data_json["USER_LIST"];

							companyName_m = (json::String const&)json_init_root["COMPANY_NAME"];

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
					CppUtils::String companyName_m;

			};


			// ------------------------------------------

			class Req_REQUEST_MT_STATUS: public AlgLibRequestRouteMtBase {
				public:
					MCTOR(Req_REQUEST_MT_STATUS, "REQUEST_MT_STATUS");

			
			};

			// --------------------

			class Req_UPDATE_USER_ACCESS_TIME: public AlgLibRequestRouteMtBase {
				public:
					MCTOR(Req_UPDATE_USER_ACCESS_TIME, "UPDATE_USER_ACCESS_TIME");
					
					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						
						json::Array mt_instances_arr_json;
						AlgLibRequestRouteMtBase::serializeBase(json_init_root);

						json::Array user_lsit_arr_json;

						for(auto const& user: users_m) {
							json::Object user_json;
							user_json["NAME"] = json::String(user);
							user_lsit_arr_json.Insert(user_json);


						}

						json::Object data_json;
						data_json["USER_LIST"] = user_lsit_arr_json;
						json_init_root["DATA"] = data_json;

						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestRouteMtBase::deserializeBase(json_init_root);

							json::Object const& data_json = json_init_root["DATA"];
							json::Array const& user_arr_json  = data_json["USER_LIST"];

							users_m.clear();

							for(int i = 0; i < user_arr_json.Size(); i++) {
								json::Object const& obj_i = user_arr_json[i];

								users_m.push_back( (json::String const&) obj_i["NAME"] );
							}
					}

					// data
					CppUtils::StringList users_m;
			};

			// -------------------------------------
			
			class Req_G_KILL_MT_PROCESS: public AlgLibRequestNonRouteMtBase {

				public:
					MCTOR(Req_G_KILL_MT_PROCESS, "G_KILL_MT_PROCESS");
					
					virtual json::Object toJson() const
					{
				
						json::Object json_init_root;
						AlgLibRequestNonRouteMtBase::serializeBase(json_init_root);

						json_init_root["MT_INSTANCE"] = json::String(mtInstance_m);
						
						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
							// serilaize metadata
							AlgLibRequestNonRouteMtBase::deserializeBase(json_init_root);

							mtInstance_m = (json::String const&)json_init_root["MT_INSTANCE"];
					}

					CppUtils::String mtInstance_m;


			};



			
	};

	

}; // end of ns

#endif