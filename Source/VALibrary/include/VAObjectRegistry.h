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

#ifndef IW_VABASE_OBJECT_REGISTRY
#define IW_VABASE_OBJECT_REGISTRY

#include <VABaseDefinitions.h>
#include <VABase.h>
#include <VAStruct.h>

#include <map>
#include <string>
#include <vector>

// Forwards
class CVAObject;

//! Registry class for objects
/**
  * Handles VA object registration. Objects will be callable via object / module interface.
  */
class VABASE_API CVAObjectRegistry
{
public:
	//! Default constructor
	CVAObjectRegistry();

	//! Destructor
	~CVAObjectRegistry();

	//! Clear objects. Deregisters all modules.
	void Clear();

	//! Registers an object with the registry and returns its ID
	/**
	  * @param[in] pObject Object pointer
	  * @return Object identifier
	  */
	int RegisterObject( CVAObject* pObject );

	//! Deregisters an object from the registry
	/**
	  * @param[in] pObject Object pointer
	  */
	void UnregisterObject( CVAObject* pObject );

	//! Query the IDs of all objects
	void GetObjectIDs( std::vector< int >& viIDs ) const;

	//! Query information on all objects
	/**
	  * @param[in] viInfos Object infos
	  */
	void GetObjectInfos( std::vector< CVAObjectInfo >& viInfos ) const;

	//! Finds an object by its ID
	/**
	  * @param[in] iID Object identifier
	  * @return Pointer to the object, NULL if not found
	  */
	CVAObject* FindObjectByID( const int iID ) const;

	//! Finds the object with the given name
	/**
	  * @param[in] sName Object name
	  * @return Pointer to the object, NULL if not found
	  * @note Object names are case-sensitive
	  */
	CVAObject* FindObjectByName( const std::string& sName ) const;

	//! Returns an object by its ID
	/**
	  * @param[in] iID Object identifier
	  * @return Pointer to the object
	  * @note Throws an exception "invalid object ID" in case the object does not exist
	  */
	CVAObject* GetObjectByID( const int iID ) const;

	//! Returns the object with the given name
	/**
	  * @param[in] sName Object name
	  * @return Pointer to the object, if existing,
	  * @note Throws an exception "unknown object" in case the object does not exist
	  * @note Object names are case-sensitive
	  */
	CVAObject* GetObjectByName( const std::string& sName ) const;

	//! Calls an object determined by its ID
	/**
	  * @param[in] iID Object identifier
	  * @param[in] oArgs Object call arguments
	  * @returns Throws an "invalid object ID" exception, if the object does not exist
	  */
	CVAStruct CallObjectByID( const int iID, const CVAStruct& oArgs ) const;

	//! Calls an object determined by its name
	/**
	  * @param[in] sName Object name
	  * @param[in] oArgs Object call arguments
	 * @returns Throws an "unknown object" exception, if the object does not exist
	 */
	CVAStruct CallObjectByName( const std::string& sName, const CVAStruct& oArgs ) const;

private:
	std::vector< CVAObject* > m_vpObjects;				//!< Object table (O(1) lookups!)
	std::map< std::string, CVAObject* > m_mpObjects;	//!< Fast name search structure (O(log N) queries!)
	size_t m_nObjects;									//!< Number of registered objects
	int m_iIDLast;										//!< ID counter

	friend class CVAObject;
};

#endif // IW_VABASE_OBJECT_REGISTRY
