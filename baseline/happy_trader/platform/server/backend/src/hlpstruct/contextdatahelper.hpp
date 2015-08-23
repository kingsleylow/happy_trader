#ifndef _HLPSTRUCT_CONTEXTDATAHELPER_INCLUDED
#define _HLPSTRUCT_CONTEXTDATAHELPER_INCLUDED

#include "hlpstructdefs.hpp"
#include "commonhelperstructs.hpp"



#define CAST_OPS(T)	\
	typedef HlpStruct::Member<T> ThisTypeDecalration;	\
	ThisTypeDecalration& operator = (T const& src)	\
	{	\
			get() = src;	\
			return *this;	\
	};	\
	operator T& ()	\
	{	\
		return get();	\
	};		\
	operator T const& () const	\
	{	\
		return get();	\
	};

namespace HlpStruct {
	/**
	* Helpers for member vars in context - to serialize
	*/

	
	
	
	template<class T>
	class MemberBase
	{
		public:
		MemberBase()
		{
		}

		virtual ~MemberBase()
		{
		}

		T& operator = (T const& src)
		{
			get() = src;
			return *this;
		}

		inline T& get()
		{
			return data_m;
		}

		inline T const& get() const
		{
			return (T const&)data_m;
		}

		virtual CppUtils::String toString() const = 0;
		
		
	private:

		T data_m;
		
	};


	template<class T>
	class Member;
	

	template<>
	class Member<int>: public MemberBase<int>
	{
		public:
		Member()
		{
			get() = -1;
		};

		CAST_OPS(int)

		 virtual CppUtils::String toString() const
		 {
			 return CppUtils::long2String(get());
		 }

		 
	};

	template<>
	class Member<bool>: public MemberBase<bool>
	{
		public:
		Member()
		{
			get() = false;
		};
		CAST_OPS(bool)
		
		 virtual CppUtils::String toString() const
		 {
			 return CppUtils::bool2String(get());
		 }
	};

	template<>
	class Member<long>: public MemberBase<long>
	{
		public:
		Member()
		{
			get() = -1;
		};

		CAST_OPS(long)

		 virtual CppUtils::String toString() const
		 {
			 return CppUtils::long2String(get());
		 }
	};

	template<>
	class Member<double>: public MemberBase<double>
	{
		public:
		Member()
		{
			get() = -1.0;
		};

		CAST_OPS(double)

		virtual CppUtils::String toString() const
		{
			 return CppUtils::float2String(get());
		}
	};

	template<>
	class Member<CppUtils::String>: public MemberBase<CppUtils::String>
	{
	public:
		Member()
		{
			get() = "";
		};

		CAST_OPS(CppUtils::String)

		virtual CppUtils::String toString() const
		{
			 return get();
		}
	};

	template<>
	class Member<CppUtils::StringList>: public MemberBase<CppUtils::StringList>
	{
	public:
		Member()
		{
			
		};

		CAST_OPS(CppUtils::StringList)

		virtual CppUtils::String toString() const
		{
			CppUtils::String r;
			
			r.append("[ ");
			for(int i = 0; i < get().size(); i++) {
				r.append(" ( ").append( get()[i] ).append(" ) ");
			}
			r.append("] ");
			return r;
		}
	};

	template<>
	class Member<CppUtils::IntList>: public MemberBase<CppUtils::IntList>
	{
	public:
		Member()
		{
			
		};

		CAST_OPS(CppUtils::IntList)

		virtual CppUtils::String toString() const
		{
			CppUtils::String r;
			
			r.append("[ ");
			for(int i = 0; i < get().size(); i++) {
				r.append(" ( ").append( CppUtils::long2String(get()[i])).append(" ) ");
			}
			r.append("] ");
			return r;	 
		}
	};

	template<>
	class Member<CppUtils::DoubleList>: public MemberBase<CppUtils::DoubleList>
	{
	public:
		Member()
		{
			
		};

		CAST_OPS(CppUtils::DoubleList)

		virtual CppUtils::String toString() const
		{
			CppUtils::String r;
			
			r.append("[ ");
			for(int i = 0; i < get().size(); i++) {
				r.append(" ( ").append( CppUtils::float2String(get()[i])).append(" ) ");
			}
			r.append("] ");
			return r;		 
		}
	};


// ------------------
// this is an additional specialization
	template<>
	class Member<WorkTimeHolder>: public MemberBase<WorkTimeHolder>
	{
		public:
		Member()
		{
		};

		CAST_OPS(WorkTimeHolder)
		
		virtual CppUtils::String toString() const
		{
			
			 return "NOT AVAILABLE";
		}
	};

	/*
	template<>
	class Member<BrkLib::TradeDirection>: public MemberBase<BrkLib::TradeDirection>
	{
		public:
		Member()
		{
			get() = BrkLib::TD_NONE;
		};
		
		virtual CppUtils::String toString() const
		{
			
			return BrkLib::tradeDirection2String( get() );
		}
	};
	*/



// --------------
// 
	



// ------------------------------------------------
//

template<int n, int n_max, class OuterClass>
struct ParamHolderSimpleDescriptor  
{
	template< bool Condition, int n, int n_max, class ParamHolderSimpleDescriptor, class OuterClass >
	class If;

	template< int n, int n_max,  class ParamHolderSimpleDescriptor, class OuterClass >
	class If<true,  n,  n_max,   ParamHolderSimpleDescriptor, OuterClass> {
	public:
		static inline CppUtils::String exec(OuterClass const& thisPtr) {
				// true
			return HlpStruct::ParamHolderSimpleDescriptor<n+1, n_max,  OuterClass >::toDescriptionString(thisPtr);
		 }
	};

	template<int n, int n_max,class ParamHolderSimpleDescriptor, class OuterClass>
	class If< false,  n,  n_max,  ParamHolderSimpleDescriptor, OuterClass > {
	public:
			static inline CppUtils::String exec(OuterClass const& thisPtr) {
				// false
				return "";
			}
	};


 // -------------------------------------------------

	static inline CppUtils::String toDescriptionString(OuterClass const& thisPtr)
	{
		CppUtils::String r;
		
	
		r.append(thisPtr.getMemberDescription<n>());
		// the next
		r.append("\n");
		r.append( ParamHolderSimpleDescriptor::If< n<n_max, n, n_max, ParamHolderSimpleDescriptor, OuterClass>::exec(thisPtr) );
		

		return r;
	}

};


// ------------------------------------------------------

#define PARAM_ENTRY_SEQ(N, TYPE, NAME_VAR)	\
			HlpStruct::Member<TYPE> NAME_VAR;	\
			template<> CppUtils::String getMemberDescription<N>()	const\
			{	\
				CppUtils::String r;	\
				r.append(#NAME_VAR);	\
				r.append("=[");	\
				r.append(NAME_VAR.toString());	\
				r.append("]");	\
				return r;	\
			};	\
				



// ----------------------------------------

//Setting this to a too big value may cause compiler errors and/or make complation slow
#define MAX_PARAMS_COUNT 30


//Function only needs to be declared because it's never realy called, only in sizeof which is
//evaluated at compile time and therefor does not create an external reference
//It is used to upcasted all counter varibales to a certain counter. It starts at 1
//because empty classes are 1 byte in size
#define INIT_COUNTER	\
template<int n>	\
class counter__ : public counter__<n-1>{	\
	int i;		\
public:	\
 counter__(int){}	\
 counter__(){}	\
};	\
\
template<>	\
class counter__<0>{	\
public:	\
 counter__(int){}	\
 counter__(){}	\
};	\
static counter__<1> get_counter(counter__<1>);

#define GET_COUNTER_VAL (sizeof(get_counter(counter__<MAX_PARAMS_COUNT+1>(0)))/sizeof(int)-1)
//Gets the value using GET_COUNTER_VAL and decares another get_counter function
//so that the counter objects don't need to be upcasted as high anymore and therefore
//have bigger size
#define INC_COUNTER_VAL static counter__<GET_COUNTER_VAL+2> get_counter(counter__<GET_COUNTER_VAL+2>)
#define ADD_COUNTER_VAL(n) static counter__<GET_COUNTER_VAL+1+n> get_counter(counter__<GET_COUNTER_VAL+1+n>)


// ----------------------------------------
#define BEGIN_PARAM_ENTRIES_DECLARE(THIS_CLASS)	\
	private:	\
	INIT_COUNTER;\
	public:	\
	typedef THIS_CLASS ThisClass;	\
	template<int n> CppUtils::String getMemberDescription() const;
	
	

#define PARAM_ENTRY_DECLARE(TYPE, NAME_VAR)	\
	private:	\
	enum CAT(CurSequence,NAME_VAR) {	\
		CAT(CurSequenceValue,NAME_VAR) = GET_COUNTER_VAL	\
	};	\
	public:	\
	PARAM_ENTRY_SEQ(CAT(CurSequenceValue,NAME_VAR), TYPE, NAME_VAR)	\
	private:	\
	INC_COUNTER_VAL;
	

#define END_PARAM_ENTRIES_DECLARE()	\
	public:	\
	CppUtils::String toDescriptionString() const	\
	{	\
		return HlpStruct::ParamHolderSimpleDescriptor<0, GET_COUNTER_VAL - 1, ThisClass >::toDescriptionString(*this);	\
	};

	

}; // end of ns 

#endif