
/** 

		Header file declaring functor template classes.
*/


#ifndef _CPPUTILS_FUNCTOR_INCLUDED
#define _CPPUTILS_FUNCTOR_INCLUDED

#include "uid.hpp"


namespace CppUtils {



// =======================================================
// Void version w/ void return
// =======================================================

/** Non-template base class for functors without argument
		and void return type.

	
*/
class VoidVoidFuncBase
{
  public:

		virtual ~VoidVoidFuncBase (
		)
		{
		};

		virtual void evaluate(
		) const = 0;
};


/** Template class for class member function functors without argument
		and void return type.

	
*/
template<class C> class VoidVoidMemFunc : public VoidVoidFuncBase
{
	  void (C::*fun_m)();
		C& obj_m;


  public:

		explicit VoidVoidMemFunc(
			void (C::*fun)(),
			C& obj
		) :
			fun_m( fun ),
			obj_m( obj )
		{
		}

		void evaluate(
		) const
		{
			(&obj_m->*fun_m)();
		}
};



/** Non-template class for free-function functors without
		argument and void return type.

	
*/
class VoidVoidFunc : public VoidVoidFuncBase
{
	  void (*fun_m)();


	public:

		explicit VoidVoidFunc(
			void (*fun)()
		) :
			fun_m(fun)
		{
		}

		void evaluate(
		) const
		{
			(*fun_m)();
		}
};




// =======================================================
// Void version w/ return type
// =======================================================
/** Template base class for functors without argument
		and template return type.

		will be autmatically removed
		
*/
template <class R> class VoidFuncBase
{
  public:

		VoidFuncBase()
		{
			uid_m.generate();
		};

		virtual ~VoidFuncBase (
		)
		{
		};

		virtual R operator() (
		) const = 0;

		Uid const& getUid() const
		{
			return uid_m;
		}


private:

	Uid uid_m;
};


/** Template class for member function functors without argument
		and template return type.

	
*/
template<class R, class C> class VoidMemFunc : public VoidFuncBase<R>
{
	  R(C::*fun_m)();
		C& obj_m;


  public:

		explicit VoidMemFunc(
			R(C::*fun)(),
			C& obj
		) :
			fun_m( fun ),
			obj_m( obj )
		{
		}

		R evaluate(
		) const
		{
			return (&obj_m->*fun_m)();
		}
};



/** Template class for free-function functors without argument
		and template return type.

	
*/
template<class R> class VoidFunc : public VoidFuncBase<R>
{
	  R(*fun_m)();


	public:

		explicit VoidFunc(
			R(*fun)()
		) :
			fun_m(fun)
		{
		}

		R evaluate(
		) const
		{
			return (*fun_m)();
		}
};





// =======================================================
// Unary version w/o return type
// =======================================================

/** Template base class for functors with single template argument
		and no return type.

		\author Olaf Lenzmann
*/
template <class X> class VoidUnaryFuncBase
{
  public:

		virtual ~VoidUnaryFuncBase (
		)
		{
		};

		virtual void evaluate(
			X
		) const = 0;
};


/** Template class for member-function functors with single
		template argument and no return type.

		
*/
template<class C, class X> class VoidUnaryMemFunc : public VoidUnaryFuncBase<X>
{
	  void(C::*fun_m)(X);
		C& obj_m;


  public:

		explicit VoidUnaryMemFunc(
			void(C::*fun)(X),
			C& obj
		) :
			fun_m( fun ),
			obj_m( obj )
		{
		}

		void evaluate(
			X x
		) const
		{
			(&obj_m->*fun_m)(x);
		}
};



/** Template class for member-function functors with single
		template argument and no return type.

		
*/
template<class X> class VoidUnaryFunc : public VoidUnaryFuncBase<X>
{
	  void(*fun_m)(X);


	public:

		explicit VoidUnaryFunc(
			void(*fun)(X)
		) :
			fun_m(fun)
		{
		}

		void evaluate(
			X x
		) const
		{
			(*fun_m)(x);
		}
};





// =======================================================
// Unary version
// =======================================================

/** Template base class for functors with single template argument
		and different template return type.

		
*/
template <class R, class X> class UnaryFuncBase
{
  public:

		virtual ~UnaryFuncBase (
		)
		{
		};

		virtual R evaluate(
			X
		) const = 0;
};


/** Template class for member-function functors with single
		template argument and different template return type.

	
*/
template<class R, class C, class X> class UnaryMemFunc : public UnaryFuncBase<R,X>
{
	  R(C::*fun_m)(X);
		C& obj_m;


  public:

		explicit UnaryMemFunc(
			R(C::*fun)(X),
			C& obj
		) :
			fun_m( fun ),
			obj_m( obj )
		{
		}

		R evaluate(
			X x
		) const
		{
			return (&obj_m->*fun_m)(x);
		}
};



/** Template class for member-function functors with single
		template argument and different template return type.

		
*/
template<class R, class X> class UnaryFunc : public UnaryFuncBase<R,X>
{
	  R(*fun_m)(X);


	public:

		explicit UnaryFunc(
			R(*fun)(X)
		) :
			fun_m(fun)
		{
		}

		R evaluate(
			X x
		) const
		{
			return (*fun_m)(x);
		}
};






// =======================================================
// Binary version
// =======================================================
/** Template base class for functors with two template argument
		and different template return type.

	
*/
template <class R, class X, class Y> class BinaryFuncBase
{
  public:

		virtual ~BinaryFuncBase (
		)
		{
		};

		virtual R evaluate(
			X,
			Y
		) const = 0;
};


/** Template class for member-function functors with two template argument
		and different template return type.

		
*/
template<class R, class C, class X, class Y> class BinaryMemFunc : public BinaryFuncBase<R,X,Y>
{
	  R(C::*fun_m)(X,Y);
		C& obj_m;


  public:

		explicit BinaryMemFunc(
			R(C::*fun)(X,Y),
			C& obj
		) :
			fun_m( fun ),
			obj_m( obj )
		{
		}

		R evaluate(
			X x,
			Y y
		) const
		{
			return (&obj_m->*fun_m)(x, y);
		}
};



/** Template class for free-function functors with two template argument
		and different template return type.

		
*/
template<class R, class X, class Y> class BinaryFunc : public BinaryFuncBase<R,X,Y>
{
	  R(*fun_m)(X,Y);


	public:

		explicit BinaryFunc(
			R(*fun)(X,Y)
		) :
			fun_m(fun)
		{
		}

		R evaluate(
			X x,
			Y y
		) const
		{
			return (*fun_m)(x, y);
		}
};




// End of namespace
}



#endif // _CPPUTILS_FUNCTOR_INCLUDED
