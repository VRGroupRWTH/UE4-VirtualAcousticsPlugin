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

#ifndef IW_VANET_DEFINITIONS
#define IW_VANET_DEFINITIONS

#if ( defined WIN32 ) && !( defined VANET_STATIC )
	#ifdef VANET_EXPORTS
		#define VANET_API __declspec( dllexport )
	#else
		#define VANET_API __declspec( dllimport )
	#endif
#else
	#define VANET_API
#endif

// Disable STL template-instantiiation warning with DLLs for Visual C++
#if defined (_MSC_VER)
#pragma warning(disable: 4251)
#endif

#ifndef VANET_DEFAULT_PORT
#define VANET_DEFAULT_PORT 12340
#endif

#endif // IW_VANET_DEFINITIONS
