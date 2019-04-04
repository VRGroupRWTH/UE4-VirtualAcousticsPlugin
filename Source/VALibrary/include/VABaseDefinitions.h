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

#ifndef IW_VABASE_DEFINITIONS
#define IW_VABASE_DEFINITIONS

#if ( defined WIN32 ) && !( defined VABASE_STATIC || defined VA_STATIC )
#ifdef VABASE_EXPORTS
#define VABASE_API __declspec( dllexport )
#define VABASE_IMPL_TEMPLATE
#else
#define VABASE_API __declspec( dllimport )
#define VABASE_IMPL_TEMPLATE
#endif
#else
#define VABASE_API
#define VABASE_IMPL_TEMPLATE
#endif

#if defined( _MSC_VER ) || defined( WIN32 )
#pragma warning( disable: 4251 ) // Disable STL template-instantiation warning with DLLs for Visual C++
#pragma warning( disable: 4201 ) // Non-compliant union definition of VAVec3.comp
#endif

// Define necessary typedef from stdint.h for Microsoft compilers before Visual C++ 2010
#if defined( _MSC_VER ) && _MSC_VER < 1600

// Implement posix type declarations under Microsoft Visual C/C++ Compiler (unless defined elsewhere)
#ifndef DEF_POSIX_TYPES_FOR_MSC
#define DEF_POSIX_TYPES_FOR_MSC

typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;

typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#endif // DEF_POSIX_TYPES_FOR_MSC

#else
#include <stdint.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#endif // IW_VABASE_DEFINITIONS
