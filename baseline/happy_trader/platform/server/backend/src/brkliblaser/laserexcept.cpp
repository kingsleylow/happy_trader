#include "laserexcept.hpp"

namespace BrkLib {

//=============================================================
// Class OperationFailed
//=============================================================
const CppUtils::String LaserFailed::exceptionType_scm = "Laser Failed";
//======================================
// Creators
//======================================
LaserFailed::LaserFailed (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		CppUtils::String const &arg
	) :
		Exception( msg, ctx, arg )
{
}

LaserFailed::LaserFailed (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		long const arg
	) :
		Exception( msg, ctx, arg )
{
}

LaserFailed::LaserFailed (
		CppUtils::String const &msg,
		CppUtils::String const &ctx,
		float const arg
	) :
		Exception( msg, ctx, arg )
{
}
//======================================
// Accessor
//======================================
CppUtils::String LaserFailed::type (
	) const
{
	return exceptionType_scm;
}

CppUtils::String LaserFailed::addInfo() const
{
	return "N/A";
}

};
