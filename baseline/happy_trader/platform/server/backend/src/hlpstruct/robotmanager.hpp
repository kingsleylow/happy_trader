#ifndef _HLPSTRUCT_ROBOTMANAGER_INCLUDED
#define _HLPSTRUCT_ROBOTMANAGER_INCLUDED

#include "hlpstructdefs.hpp"
#include "xmlparameter.hpp"
#include "commonhelperstructs.hpp"

#define ROBOTMANAGER "ROBOTMANAGER"

namespace HlpStruct {

	
	/**
	* Class incapsulationg work with parameterized robot
	- helper for allowing calling different robots in a sequence with different parameters
	*/

	/*
	template<class TheClass>
	class SequenceRobotBase {
		public:
		typedef TheClass ImplementationClass;

		SequenceRobotBase()
		{
			uid_m.generate();
		}

		virtual ~SequenceRobotBase()
		{
		}

		CppUtils::Uid const& getId() const
		{
			return uid_m;
		}

	private:
		CppUtils::Uid uid_m;	


		
	};

	// --------------------------------

	namespace  Inqueue {
		class ContextAccessor;
	};
	namespace BrkLib {
		class BrokerBase;
	};

	template<class TheClass>
	class RobotManager
	{
	public:
		RobotManager()
		{
		};

		~RobotManager()
		{
			clear();
		};

		TheClass& registerRobot()
		{
			 TheClass* r = new TheClass();
			 list_m.push_back( r );

			 return r;
		}

		void clear()
		{
			for(int i =0; i < list_m.size(); i++) {
				try {
					delete list_m[ i ];
				}
				catch(CppUtils::Exception& e)
				{
					LOG(MSG_ERROR, "RobotManager(...)", "Exception on removing SequenceRobotBase instance: " << e.message());
				}
				catch(...)
				{
					// ignore
					LOG(MSG_ERROR, "RobotManager(...)", "Unknown exception on removing SequenceRobotBase instance: ");
				}
			}

			list_m.clear();
		}

		void onDataArrived(
			HlpStruct::RtData const& rtdata, 
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
		{
			for(int i =0; i < list_m.size(); i++) {
				list_m[i]->onDataArrived(rtdata, historyaccessor, descriptor, brokerlib, runName, comment);
			}
		};

		void onLevel2DataArrived(
			HlpStruct::RtLevel2Data const& level2data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
		{
			for(int i =0; i < list_m.size(); i++) {
				list_m[i]->onLevel2DataArrived(level2data, historyaccessor, descriptor, brokerlib, runName, comment);
			}
		};

		void onLevel1DataArrived(
			HlpStruct::RtLevel1Data const& level1data,
			Inqueue::ContextAccessor& historyaccessor, 
			HlpStruct::TradingParameters const& descriptor,
			BrkLib::BrokerBase* brokerlib,
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
		{
			for(int i =0; i < list_m.size(); i++) {
				list_m[i]->onLevel1DataArrived(level1data, historyaccessor, descriptor, brokerlib, runName, comment);
			}
		};

		void onLibraryInitialized(
			HlpStruct::AlgBrokerDescriptor const& descriptor, 
			BrkLib::BrokerBase* brokerlib, 
			CppUtils::String const& runName,
			CppUtils::String const& comment
		)
		{
			for(int i =0; i < list_m.size(); i++) {
				list_m[i]->onLibraryInitialized(descriptor, runName, comment);
			}
		}


		void onLibraryFreed()
		{
			for(int i =0; i < list_m.size(); i++) {
				list_m[i]->onLibraryFreed();
			}
		}

	private:

		vector<TheClass*> list_m;
	};

	*/

}; // end of ns

#endif