#ifndef _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_OUT_INCLUDED
#define _ALGLIBMT2_EXCHANGE_PACKETS_ALGLIB_OUT_INCLUDED

#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"
#include "exchange_packets_mt.hpp"



#include "exchange_packets_base.hpp"
#include "shared/cpputils/src/cpputils.hpp"

// this describes out events to java layer
// in the form of json as well which will be later wrapped into XMLEvent

namespace AlgLib {
	class Alglib_Out_Dialog {
	public:
		static BaseDlgEntity* creator(json::Object& json_init_root) {
			CppUtils::String command = CppUtils::toUpperCase((json::String const&)json_init_root["TYPE"]);

			BEGIN_CREATOR_DLG_ENTRY(command.c_str());

			CREATOR_DLG_ENTRY(HeartBeatOut);
			CREATOR_DLG_ENTRY(MtDataPacketOut);
		
			END_CREATOR_DLG_ENTRY()


				return NULL;
		}

		// base
		class OutPacketBase : public BaseDlgEntity {
		public:
			virtual ~OutPacketBase()
			{
			}
			  
			// some common serialization work
			void serializeBase(json::Object& json_init_root) const
			{
				json_init_root["TYPE"]  =json::String(getCommandName());

			}

			void deserializeBase(json::Object const& json_init_root)
			{
			}

			// need to give toJson and from Json default implementatiosn
			virtual json::Object toJson() const
			{
				json::Object json_init_root;
				serializeBase(json_init_root);

				// need proceed upper level
				return json_init_root;
			}

			virtual void fromJson(json::Object const& json_init_root)
			{
				// serilaize metadata
				deserializeBase(json_init_root);
			}



		};

		// -----------------------------------

		// just a simple heart beat
		class HeartBeatOut : public OutPacketBase {
			public:
				MCTOR(HeartBeatOut, "HEARTBEAT");

			
		};

		// ----------------------------------

		// event notification
		class MtStatusOut : public OutPacketBase {
				public:
					enum Status {
						MT_CONNECTED = 1,
						MT_DISCONNECTED = 2
					};
					MCTOR(MtStatusOut, "MT_STATUS");

					virtual json::Object toJson() const
					{
						json::Object json_init_root;
						serializeBase(json_init_root);

						// just fully serialize this
						json_init_root["MT_INSTANCE"] = (json::String)mtInstance_m;
						json_init_root["STATUS"] = (json::Number)status_m;

						// need proceed upper level
						return json_init_root;
					}

					virtual void fromJson(json::Object const& json_init_root)
					{
						// serilaize metadata
						deserializeBase(json_init_root);

						mtInstance_m = (json::String const&)json_init_root["MT_INSTANCE"];
						status_m = (int)(json::Number const&)json_init_root["STATUS"];
					}

					int status_m;
					CppUtils::String mtInstance_m;
		};

		// just a simple heart beat
		class MtDataPacketOut : public OutPacketBase {
			public:
				MCTOR(MtDataPacketOut, "FULL_MT_PACKET");


			virtual json::Object toJson() const
			{
				json::Object json_init_root;
				serializeBase(json_init_root);

				// just fully serialize this
				json_init_root["EVENT_DATA"] = mtData_m.toJson();

				// need proceed upper level
				return json_init_root;
			}

			virtual void fromJson(json::Object const& json_init_root)
			{
				// serilaize metadata
				deserializeBase(json_init_root);

				mtData_m.fromJson( json_init_root["EVENT_DATA"] );
			}


			// received from MT
			MtProxy_AlgLib_Dialog::Mtproxy_Data mtData_m;
		};


	};
};

#endif