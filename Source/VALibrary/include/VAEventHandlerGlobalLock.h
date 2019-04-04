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

#ifndef IW_VABASE_EVENT_HANDLER_GLOBAL_LOCK
#define IW_VABASE_EVENT_HANDLER_GLOBAL_LOCK

#include <VABaseDefinitions.h>

//! Global synchronization token for event handler operations
/**
  * This singleton class realizes a global synchronization token
  * that can be used to realize mutual exclusive operations regarding
  * event handlers, like attaching/detaching event handlers to
  * event handler registrys and destruction of event handlers.
  */
class VABASE_API IVAEventHandlerGlobalLock
{
public:
	//! Returns the singleton instance
	/**
	  * @return Global lock for event handling
	  */
	static IVAEventHandlerGlobalLock& GetInstance();

	//! Lock (blocking wait, no try)
	virtual void Lock() const = 0;

	//! Unlock
	virtual void Unlock() const = 0;

protected:
	//! Protected default constructor
	inline IVAEventHandlerGlobalLock() {};

	//! Protected destructor
	virtual inline ~IVAEventHandlerGlobalLock() {};
};

#endif // IW_VABASE_EVENT_HANDLER_GLOBAL_LOCK
