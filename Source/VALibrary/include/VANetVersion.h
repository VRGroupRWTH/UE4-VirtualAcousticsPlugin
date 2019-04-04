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

#ifndef IW_VANET_VERSION
#define IW_VANET_VERSION

#include <VANetDefinitions.h>

#include <string>

//! Net version
/**
  * Describes the network version of library.
  *
  */
class VANET_API CVANetVersionInfo
{
public:
	std::string sVersion;				//!< Version as string (e.g. "1.02")
	std::string sDate;					//!< Date of creation / publiching date
	std::string sFlags;					//!< List of properties (flags) (e.g. "debug", "SSE2")
	std::string sComments;				//!< Additional comments

	//! Formatted string
	/**
	  * @return Formatted string
	  */
	std::string ToString() const;
};

//! Get version of network library
/**
  * @param[out] pVersionInfo Object where to store version info
  */
void VANET_API GetVANetVersionInfo( CVANetVersionInfo* pVersionInfo );

#endif // IW_VANET_VERSION
