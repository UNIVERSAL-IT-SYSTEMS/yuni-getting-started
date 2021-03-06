#pragma once




namespace Yuni
{
namespace Policy
{

/*!
** \brief Conversion policies
** \ingroup Policies
*/
namespace Conversion
{


	/*!
	** \ingroup Policies
	*/
	struct Allow
	{
		enum { allow = true };
		static void swapPointer(Allow&) {}
	};


	/*!
	** \ingroup Policies
	*/
	struct Disallow
	{
		//! Default constructor
		Disallow() {}
		/*!
		** \brief Copy constructor
		**
		** It is possible to initialize a `Disallow` policy with an `Allow` policy
		*/
		Disallow(const Allow&) {}

		enum { allow = false };

		static void swapPointer(Disallow&) {}
	};



} // namespace Conversion
} // namespace Policy
} // namespace Yuni

