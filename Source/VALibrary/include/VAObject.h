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

#ifndef IW_VABASE_OBJECT
#define IW_VABASE_OBJECT

#include <VABaseDefinitions.h>
#include <VABase.h>
#include <VAStruct.h>

//! Base class for objects with identity and message interface
/**
  * In VA, anything can be an object and can be called via the module call interface during runtime.
  * These calls are usually not thread-safe.
  */
class VABASE_API CVAObject
{
public:
	//! Default constructor
	CVAObject();

	//! Initialization constructor
	/**
	  * @param[in] pszName Character type name initialization
	  */
	CVAObject( const char* pszName );
	
	//! Initialization constructor
	/**
	  * @param[in] sName String type name initialization
	  */
	CVAObject( const std::string& sName );

	//! Destructor
	virtual ~CVAObject();

	//! Returns the ID of the object
	/**
	  * @return Object identifier integer
	  */
	int GetObjectID() const;

	//! Returns the name of the object
	/**
	  * @return Object name
	  */
	std::string GetObjectName() const;

	//! Returns information on the object
	/**
	  * @return Object info
	  */
	virtual CVAObjectInfo GetObjectInfo() const;

	//! Call the object with a message and capture return message
	/**
	  * @param[in] oArgs Object call arguments as CVAStruct
	  * @return Object call return value as CVAStruct
	  */
	virtual CVAStruct CallObject( const CVAStruct& oArgs ) = 0;

protected:
	// Important: An object may not change its name after it is registered
	/**
	  * @param[in] sName Name of new object
	  * @note This may only be called from an object registry
	  */
	void SetObjectName( const std::string& sName );

private:
	int m_iObjectID; //!< Object identifier
	std::string m_sObjectName; //!< Object name (should not be changed during runtime)

	//! Object identifier (integer) setter 
	/**
	  * @param[in] iID Object identifier number
	  * @note This may only be called from an object registry
	  */
 	void SetObjectID( const int iID );

	friend class CVAObjectRegistry;
};

#endif // IW_VABASE_OBJECT
