#ifndef _BRKLIB_HELPERFUN_INCLUDED
#define _BRKLIB_HELPERFUN_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"


namespace BrkLib {

	BRKLIB_EXP CppUtils::String posTypeToString(BrkLib::PosType const& posType);

	BRKLIB_EXP TradeDirection posTypeToDirection(BrkLib::PosType const& posType);

	BRKLIB_EXP TradeDirection orderTypeToDirection(BrkLib::OrderType const& orderType);

	BRKLIB_EXP CppUtils::String tradeDirection2String(TradeDirection const td);

	BRKLIB_EXP CppUtils::String tradeDirection2String(int const td);

	BRKLIB_EXP CppUtils::String orderTypeBitwise2String(int const ot);

	BRKLIB_EXP CppUtils::String orderValidity2String(OrderValidity const ov);

	// TODO - in the future we will review bitwise ops and remove this hacky functions
	BRKLIB_EXP OrderType orderTypeClearance(int const ot);

	BRKLIB_EXP CppUtils::String getOrderSmallDescription(BrkLib::Order const& order);



	




}; // end of namespace


namespace HlpStruct {
	
	// continue specialization
	template<>
	class Member<BrkLib::TradeDirection>: public MemberBase<BrkLib::TradeDirection>
	{
		public:
		Member()
		{
			get() = BrkLib::TD_NONE;
		};
		
		CAST_OPS(BrkLib::TradeDirection)

		virtual CppUtils::String toString() const
		{
			
			return BrkLib::tradeDirection2String(get());
		}
	};

	
};

#endif

