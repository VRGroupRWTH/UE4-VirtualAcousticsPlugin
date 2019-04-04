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

#ifndef INCLUDE_WATCHER_VACS_DEFINITIONS
#define INCLUDE_WATCHER_VACS_DEFINITIONS

// VA C# binding can only be used as a dynamic library
#if ( defined WIN32 ) 
#ifdef VACS_EXPORTS
#define VACS_API __declspec( dllexport )
#else
#define VACS_API __declspec( dllimport )
#endif
#else
#define VACS_API
#endif

#endif // INCLUDE_WATCHER_VACS_DEFINITIONS
