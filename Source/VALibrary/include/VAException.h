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

#ifndef IW_VABASE_EXCEPTION
#define IW_VABASE_EXCEPTION

#include <VABaseDefinitions.h>
#include <ostream>
#include <string>

//! Base class for exceptions
/**
  * This is the exception base class for all VA software components.
  * Exceptions are defined by an error code and an error message.
  */
class VABASE_API CVAException
{
public:
	//! Error codes
	enum ErrorCode 
	{
		UNSPECIFIED = 0,		//!< An error that is not specified in detail
		INVALID_PARAMETER = 1,	//!< A parameter provided is invalid
		INVALID_ID = 2,			//!< An identifier (ID) provided is invalid
		MODAL_ERROR = 3,		//!< A modal error occured
		RESOURCE_IN_USE = 4,	//!< The resource is in use
		FILE_NOT_FOUND = 5,		//!< A file was not found
		NETWORK_ERROR = 6,		//!< A networking error occured
		PROTOCOL_ERROR = 7,		//!< Behaviour violates the protocol specification
		NOT_IMPLEMENTED = 8,	//!< A specific behaviour is not implemented by a component
	};

	//! Default constructor
	CVAException();

	//! Initialization constructor
	/**
	  * @param[in] iErrorCode Error code
	  * @param[in] sErrorMessage Error text
	  */
	CVAException( const int iErrorCode, const std::string& sErrorMessage = "" );

	//! Destructor
	virtual ~CVAException();

	//! Returns the error code
	/**
	  * @return Error code, one of ErrorCode
	  */
	int GetErrorCode() const;

	//! Returns the error message
	/**
	  * @return Error text message
	  */
	std::string GetErrorMessage() const;

	//! Return a string representation of the exception
	/**
	  * @return Formatted text of exception
	  */
	std::string ToString() const;

protected:
	int m_iErrorCode; //!< Error code, one of ErrorCode
	std::string m_sErrorMessage; //!< Error text message
};

//! Macro for throwing exceptions
#define VA_EXCEPT0( ERRRORCODE ) { throw CVAException( ( CVAException::ERRRORCODE ) ); }
#define VA_EXCEPT1( ERRRORMSG ) { throw CVAException( CVAException::UNSPECIFIED, ( ERRRORMSG ) ); }
#define VA_EXCEPT2( ERRORCODE, ERRRORMSG ) { throw CVAException( ( CVAException::ERRORCODE ) , ( ERRRORMSG ) ); }
#define VA_EXCEPT_NOT_IMPLEMENTED { throw CVAException( ( CVAException::NOT_IMPLEMENTED ) , "Not implemented" ); }
#define VA_EXCEPT_NOT_IMPLEMENTED_NEXT_VERSION { throw CVAException( ( CVAException::NOT_IMPLEMENTED ) , "This new method has been introduced to the VA interface and will be implemented in the next core version" ); }
#define VA_EXCEPT_NOT_IMPLEMENTED_FUTURE_VERSION { throw CVAException( ( CVAException::NOT_IMPLEMENTED ) , "This new method has been introduced to the VA interface for future use and is currently not implemented" ); }

//! STL stream output operator
/**
  * @param[in] os Outstream object
  * @param[in] ex VA exception
  * @return Outstream object including exception text
  */
VABASE_API std::ostream& operator<<( std::ostream& os, const CVAException& ex );

#endif // IW_VABASE_EXCEPTION
