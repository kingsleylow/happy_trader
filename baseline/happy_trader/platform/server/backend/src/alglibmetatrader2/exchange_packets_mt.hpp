#ifndef _ALGLIBMT2_EXCHANGE_PACKETS_MT_INCLUDED
#define _ALGLIBMT2_EXCHANGE_PACKETS_MT_INCLUDED

#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"


/**
* This file encapsulate JSON exchange packets, so keep in hpp file to use in mt proxy project
*/
namespace AlgLib {
	class MtProxy_AlgLib_Dialog {
	public:


		static BaseDlgEntity* creator(json::Object& json_init_root) {
				CppUtils::String command = CppUtils::toUpperCase((json::String const&)json_init_root["COMMAND"]);
			
				BEGIN_CREATOR_DLG_ENTRY(command.c_str());

					CREATOR_DLG_ENTRY(Mtproxy_Initialize);
					CREATOR_DLG_ENTRY(Mtproxy_HeartBeat);
					CREATOR_DLG_ENTRY(Mtproxy_Data);
				END_CREATOR_DLG_ENTRY()


				return NULL;
		}

		// ---------------------------------------
		//base for MT proxies
		// here we can define creator map

	
		class MTProxyPacketBase : public BaseDlgEntity {
			public:
				
			virtual ~MTProxyPacketBase()
			{
			}

			// some common serialization work
			void serializeBase(json::Object& json_init_root) const
			{
				json_init_root["COMMAND"]  =json::String(getCommandName());
				json_init_root["MT_UID"]  =json::String(mtUid_m.toString());
			}

			void deserializeBase(json::Object const& json_init_root)
			{
				mtUid_m.fromString ( (json::String const& )json_init_root["MT_UID"] );
				
			}

					

			CppUtils::Uid mtUid_m;

		};

		// ------------------------------
		// derived

		class Mtproxy_Initialize: public MTProxyPacketBase		{
		public:
			
			MCTOR(Mtproxy_Initialize, "INITIALIZE");

			virtual json::Object toJson() const
			{
				
				json::Object json_init_root;
				serializeBase(json_init_root);
				json_init_root["TERMINAL_PATH"] = json::String(terminalPath_m);
				json_init_root["PID"] = json::Number(pid_m);


				// need proceed upper level
				return json_init_root;
			}

			virtual void fromJson(json::Object const& json_init_root)
			{
				deserializeBase(json_init_root);
				terminalPath_m = (json::String const&)json_init_root["TERMINAL_PATH"];
				pid_m = (json::Number const&)json_init_root["PID"];
			}

			CppUtils::String terminalPath_m;
			// process ID
			int pid_m;
		};

		class Mtproxy_HeartBeat: public MTProxyPacketBase
		{
		public:
			MCTOR(Mtproxy_HeartBeat, "HEARTBEAT");
			
			virtual json::Object  toJson() const
			{
				json::Object json_init_root;
				serializeBase(json_init_root);
				json_init_root["PENDING_SIZE"] = json::Number(pendingSize_m);

				return json_init_root;
			}

			virtual void fromJson(json::Object const& json_init_root)
			{
				
				deserializeBase(json_init_root);
				pendingSize_m = (json::Number const&)json_init_root["PENDING_SIZE"];

			}

			int pendingSize_m;
		};

		class Mtproxy_Data: public  MTProxyPacketBase
		{
		public:
			
			MCTOR(Mtproxy_Data, "DATA_PACKET");

			virtual json::Object  toJson() const
			{
				json::Object json_init_root;
				serializeBase(json_init_root);

				json_init_root["PENDING_SIZE"]  = json::Number(pendingSize_m);
				json_init_root["DATA"]  = data_m;

				return json_init_root;
			}

			virtual void fromJson(json::Object const& json_init_root)
			{
				deserializeBase(json_init_root);
				pendingSize_m = (json::Number const&)json_init_root["PENDING_SIZE"];
				data_m = (json::Object const&)json_init_root["DATA"];
			}

			int pendingSize_m;
			json::Object data_m;
		};
		
	

		
	};
};

#endif