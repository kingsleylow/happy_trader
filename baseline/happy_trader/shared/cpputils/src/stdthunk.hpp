#ifndef _CPPUTILS_STDTHUNK_INCLUDED
#define _CPPUTILS_STDTHUNK_INCLUDED

namespace CppUtils {


template< int INSTANCE, class BASE, class R, 
class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
class CallBackAdapter {

public:

	CallBackAdapter(
		BASE* baseObj,
		R (BASE::*callback)(A1, A2, A3, A4, A5, A6, A7, A8	)
	):
	baseObject_m(baseObj),
	callback_m(callback)
	{
		CallBackAdapter::_this = this;
	}

	static R CALLBACK callBackFun(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8	)
	{
		return (_this->baseObject_m->*CallBackAdapter::_this->callback_m)(a1, a2, a3, a4, a5, a6, a7, a8);
	};
	
	
	
public:

	// function ptr
	R (BASE::*callback_m)(A1, A2, A3, A4, A5, A6, A7, A8 );

	BASE* baseObject_m;
		
	__declspec(thread) static CallBackAdapter* _this;
	
};

template< int INSTANCE, class BASE, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
CallBackAdapter< INSTANCE, BASE, R, A1, A2, A3, A4, A5, A6, A7, A8 >* CallBackAdapter< INSTANCE, BASE, R, A1, A2, A3, A4, A5, A6, A7, A8 >::_this  = NULL;

}; // end of namespace

#endif