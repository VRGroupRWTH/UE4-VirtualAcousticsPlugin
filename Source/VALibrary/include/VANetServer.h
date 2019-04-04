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

#ifndef IW_VANET_SERVER
#define IW_VANET_SERVER

#include <VANetDefinitions.h>

#include <string>
#include <list>

// Forward declarations
class IVAInterface;

//! VA server interface
/**
  * This class defines an interface for a TCP/IP network server with 
  * a managed VA core.
  *
  * Use the factory method Create() to instantiate a server, then set a 
  * VA core interface instance using SetCoreInstance().
  *
  * Calls from a network client will be transmitted over TCP/IP transparently,
  * and will be forwarded to the actual core.
  *
  * The server network functionality allows multiple client connections, however
  * updates are synchronized making them potentially less efficient.
  */
class VANET_API IVANetServer
{
public:
	typedef std::pair< int, int > tPortRange; //!< Port range typedef
	typedef std::list< tPortRange > tPortList; //!< List of ranges typedef for multi client support

	//! Server state
	enum ServerState
	{
		VA_NO_ERROR = 0, //!< No error
		VA_ALREADY_INITIALIZED, //!< Already initialized
		VA_NOT_INITIALIZED, //!< Not initialized
		VA_CONNECTION_FAILED, //!< Connection failed
	};

	//! Factory method to create a server
	/**
	  * Use this method to create an instance of a server, because default constructor is protected.
	  */
	static IVANetServer* Create();

	//! Destructor
	virtual ~IVANetServer();

	//! Initialize server
	/**
	  *
	  * @param[in] sInterface		Network TCP/IP interface where server will listen
	  * @param[in]            iServerPort		Network TCP/IP socket port
	  * @param[in]            iFreePortMin		Port range beginning for communication
	  * @param[in]            iFreePortMax		Port range end for communication
	  * @param[in]            iMaxNumClients	Maximum number of allowed clients (-1 allows infinit connections)
	  * 
	  * @return Status code
	  *
	  */
	virtual int Initialize( const std::string& sInterface, const int iServerPort = VANET_DEFAULT_PORT, const int iFreePortMin = 10000, const int iFreePortMax = 11000, const int iMaxNumClients = -1 ) = 0;
	
	//! Initialize server
	/**
	  *
	  * @param[in] sInterface		Network TCP/IP interface where server will listen
	  * @param[in] iServerPort		Network TCP/IP socket port
	  * @param[in]           liFreePorts		Port range list for communication
	  * @param[in]            iMaxNumClients	Maximum number of allowed clients (-1 allows infinit connections)
	  * 
	  * @return Status code
	  *
	  */
	virtual int Initialize( const std::string& sInterface, const int iServerPort, const tPortList& liFreePorts, const int iMaxNumClients = -1 ) = 0;

	//! Finalizes server and disconnects clients
	/**
	  * @return Status code
	  */
	virtual int Finalize() = 0;

	//! Reset all connections
	virtual void Reset() = 0;

	//! Server address getter
	/**
	  * @return Server address string
	  */
	virtual std::string GetServerAdress() const = 0;
	
	//! Server port getter
	/**
	  * @return Server port
	  */
	virtual int GetServerPort() const = 0;

	//! Core instance getter
	/**
	  * Get the attached core interface pointer.
	  *
	  * @return Interface pointer, or nullptr
	  */
	virtual IVAInterface* GetCoreInstance() const = 0;
	
	//! Core instance setter
	/**
	  * If you have instantiated a VA core, use this method to attach a core
	  * to the server.
	  *
	  * @param[in] pCore Interface pointer
	  */
	virtual void SetCoreInstance( IVAInterface* pCore ) = 0;

	//! Returns true if at least one client is connected
	/**
	  * @return True, if at least one client is connected
	  */
	virtual bool IsClientConnected() const = 0;
	
	//! Returns number of connected clients
	/**
	  * @return Number of connected clients
	  */
	virtual int GetNumConnectedClients() const = 0;

	//! Returns hostname of given client
	/**
	  * @param[in] iClientIndex Client index integer
	  * @return Client host name, e.g. "127.0.0.1"
	  */
	virtual std::string GetClientHostname( const int iClientIndex ) const = 0;

	//! Server communication event class
	class CEvent
	{
	public:
		enum EventType
		{
			EVENT_NOTHING = 0, //!< No event type
			EVENT_CLIENT_CONNECT,		//!< Client initialized a connection
			EVENT_CLIENT_DISCONNECT,	//!< Client requests a disconnect
			EVENT_CLIENT_LOST			//!< Unintended client connection loss
		};

		int iType;				//!< Event type
		std::string sClient;	//!< Client string

		//! Default constructor sets event to no event type
		inline CEvent()
			: iType( EVENT_NOTHING )
		{};

		//! Constructor with event type and client string
		/**
		  * @param[in] iInitType Initialize with this event type
		  * @param[in] sClient Client string
		  */
		inline CEvent( const int iInitType, const std::string& sClient )
			: iType( iInitType )
			, sClient( sClient )
		{};
	};

	//! Server communication event handler class
	class IEventHandler
	{
	public:
		//! Destructor
		inline virtual ~IEventHandler() {};

		//! Event handler hook / callback method
		/**
		  * @param[in] oEvent Event information
		  */
		virtual void HandleEvent( const IVANetServer::CEvent& oEvent ) = 0;
	};	
	
	//! Connection event handler attachment
	/**
	  * @param[in] pHandler Pointer to handler instance
	  * @return True, if attachment possible
	  */
	virtual bool AttachEventHandler( IVANetServer::IEventHandler* pHandler ) = 0;
	
	//! Connection event handler detachment
	/**
	  * @param[in] pHandler Pointer to handler instance
	  * @return True, if detachment possible
	  */
	virtual bool DetachEventHandler( IVANetServer::IEventHandler* pHandler ) = 0;

protected:
	//! Disabled default constructor, use Create() factory method instead.
	IVANetServer();
};

#endif // IW_VANET_SERVER
