/*
 *  --------------------------------------------------------------------------------------------
 *
 *    VVV        VVV A           Virtual Acoustics (VA) | http://www.virtualacoustics.org
 *     VVV      VVV AAA          Licensed under the Apache License, Version 2.0
 *      VVV    VVV   AAA
 *       VVV  VVV     AAA        Copyright 2015-2018
 *        VVVVVV       AAA       Institute of Technical Acoustics (ITA)
 *         VVVV         AAA      RWTH Aachen University
 *
 *  --------------------------------------------------------------------------------------------
 */

#ifndef IW_VABASE_CORE_VERSION
#define IW_VABASE_CORE_VERSION

#include <VABaseDefinitions.h>
#include <string>

//! Version info data class
/**
  * Class describing version info of VA components
  */
class VABASE_API CVAVersionInfo
{
public:
	std::string sVersion;				//!< Version as string (e.g. "1.02")
	std::string sDate;					//!< Date of creation / publiching date
	std::string sFlags;					//!< List of properties (flags) (e.g. "debug", "SSE2")
	std::string sComments;				//!< Additional comments

	//! Return VA version as formatted string
	/**
	  * @return Version info as formatted string
	  */
	std::string ToString() const;
};

#endif // IW_VABASE_CORE_VERSION
