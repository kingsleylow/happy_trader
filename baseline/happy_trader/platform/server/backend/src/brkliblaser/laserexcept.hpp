#ifndef _BRKLIBLASER_LASEREXCEPT_INCLUDED
#define _BRKLIBLASER_LASEREXCEPT_INCLUDED

#include "shared/cpputils/src/cpputils.hpp"

namespace BrkLib {

//=============================================================
// Class LaserFailed
//=============================================================

class LaserFailed : public CppUtils::Exception {

public:

	//======================================
	// Creators

	LaserFailed (
		CppUtils::String const &str,
		CppUtils::String const &ctx,
		CppUtils::String const &arg
	);

	LaserFailed (
		CppUtils::String const &str,
		CppUtils::String const &ctx,
		long const arg
	);

	LaserFailed (
		CppUtils::String const &str,
		CppUtils::String const &ctx,
		float const arg
	);


	//======================================
	// Methods

	CppUtils::String type (
	) const;


	virtual CppUtils::String addInfo() const;

protected:

	//======================================
	// Creators

	LaserFailed (
	);


	//======================================
	// Variables

	/** Exception type description. */
	static const CppUtils::String exceptionType_scm;
};

}; // end of namespace


#endif