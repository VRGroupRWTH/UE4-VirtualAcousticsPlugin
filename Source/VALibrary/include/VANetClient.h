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

#ifndef IW_VANET_CLIENT
#define IW_VANET_CLIENT

#include <VANetDefinitions.h>
#include <string>

class IVAInterface;
class CVANetNetworkProtocol;

//! Networked transparend calls to VA interface view TCP/IP as a client
/**
  * This class defines an interface for network clients to IVAInterface.
  * Almost the entire interface is replicated via network.
  *
  * Create an instance using the factory method Create().
  *
  * Client creates a client-side implementation of the interface on success.
  * You can get access to the (transparently networked) interface using
  * GetCoreInstance(). Any method call will be forwarded to the corresponding
  * server.
  *
  */
class VANET_API IVANetClient
{
public:
	//! Error types
	enum ErrorType
	{
		VA_NO_ERROR = 0,				//!< No error / success
		VA_SERVER_CONNECTION_FAILED,	//!< Connection faild - wrong address or no server running
		VA_SERVER_NOT_RESPONDING,		//!< Connection established, but server is not responding
		VA_SERVICE_IN_USE,				//!< Network service is already used by another client
		VA_CONNECTION_ERROR,			//!< Unknown network error occurred
		VA_PROTOCOL_INCOMPATIBLE,		//!< Incompatible network protocol versions (declined connection)
		VA_UNKNOWN_ERROR, //!< Any other unrecognized error
	};

	//! Head channel modi
	enum HeadChannelMode
	{
		VA_HC_USE_EXISTING = 0,
		VA_HC_SEPARATE_TCP,
		VA_HC_SEPARATE_UDP
	};

	//! Exception handling modi
	enum ExceptionHandlingMode
	{
		EXC_CLIENT_THROW, //!< for all functions, immediately transmit exception and throws it
		EXC_SERVER_PRINT, //!< functions without return value immediately return, and the exception is printed on the server
	};

	//! Factory method
	/**
	  * @return Pointer to new net client
	  */
	static IVANetClient* Create();

	//! Destructor
	virtual ~IVANetClient();

	//! Return client-side interface (networked calls)
	/**
	  * @return Interface pointer
	  */
	virtual IVAInterface* GetCoreInstance() const = 0;

	//!< Server connection getter
	/**
	  * @return True, if connection is established
	  */
	virtual bool IsConnected() const = 0;

	//! Server address getter
	/**
	  * @return Server address string
	  */
	virtual std::string GetServerAddress() const = 0;

	//! Initialize client and connect server
	/**
	  * @param[in] sServerAddress Server address string
	  * @param[in] iServerPort Server port (defaults to VANet definition)
	  * @param[in] iHeadChannelMode Server port (defaults to VANet definition)
	  * @param[in] iExceptionHandlingMode Define exception mode handling
	  * @param[in] bBufferSynchronizedCommands Buffer synchronized commands on client side
	  * 
	  * @return Status code
	  *
	  */
	virtual int Initialize( const std::string& sServerAddress, const int iServerPort = VANET_DEFAULT_PORT, const int iHeadChannelMode = VA_HC_USE_EXISTING, const int iExceptionHandlingMode = EXC_CLIENT_THROW, const bool bBufferSynchronizedCommands = true ) = 0;
	
	//! Exception mode setter
	/**
	  * @param[in] nMode Exception mode
	  */
	virtual void SetExceptionHandlingMode( const int nMode ) = 0;

	//! Exception mode getter
	/**
	  * @return Exception mode
	  */
	virtual int GetExceptionhandlingMode() const = 0;

	//! Disconnect an established connection
	/**
	  * @return Status code
	  */
	virtual int Disconnect() = 0;
	
	//! Connection event class
	class CEvent
	{
	public:
		//! Event types
		enum EventTypes
		{
			EVENT_NOTHING = 0, //!< No event type
			EVENT_SERVER_DISCONNECT,	//!< Intended connection abort
			EVENT_SERVER_LOST			//<! Unintended connection loss
		};

		std::string sServer;	//!< Event server
		int iType; //!< Event type

		//! Default constructor with no event type
		inline CEvent()
			: iType( EVENT_NOTHING )
		{
		};

		//! Constructor of special type
		/**
		  * @param[in] iInitType Initialization type
		  * @param[in] sInitServer Initialization server identifier
		  */
		inline CEvent( const int iInitType, const std::string& sInitServer )
			: iType( iInitType )
			, sServer( sInitServer )
		{
		};
	};

	//! Interface for handling conection events using the provided hook / callback
	class IEventHandler
	{
	public:
		//! Destructor
		inline virtual ~IEventHandler()	{};

		//! Handler hook
		/**
		  * @param[in] oEvent Event information
		  */
		virtual void HandleEvent( const IVANetClient::CEvent& oEvent ) = 0;
	};
	
	//! Connection event handler attachment
	/**
	  * @param[in] pHandler Pointer to handler instance
	  * @return True, if attachment possible
	  */
	virtual bool AttachEventHandler( IVANetClient::IEventHandler* pHandler ) = 0;

	//! Connection event handler detachment
	/**
	  * @param[in] pHandler Pointer to handler instance
	  * @return True, if detachment possible
	  */
	virtual bool DetachEventHandler( IVANetClient::IEventHandler* pHandler ) = 0;

protected:
	//! No default construction
	IVANetClient();
};

#endif // IW_VANET_CLIENT
