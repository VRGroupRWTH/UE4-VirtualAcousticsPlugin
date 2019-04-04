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

#ifndef IW_VANET_UTILS
#define IW_VANET_UTILS

#include <VANetDefinitions.h>

#include <string>

//! Split a server string of address and port ("localhost:12340" => localhost, 12340)
/**
  * @note Raises CVAException if parsing went wrong.
  * @param[in] sServerString Coded server string aka "localhost:12340"
  * @param[out] sAddress Server adress
  * @param[out] iPort Server socket port
  * @param[in] iDefaultPort Default port in case server string has no port
  *
  */
VANET_API void SplitServerString( const std::string& sServerString, std::string& sAddress, int& iPort, const int iDefaultPort = VANET_DEFAULT_PORT );

#endif // IW_VANET_UTILS
