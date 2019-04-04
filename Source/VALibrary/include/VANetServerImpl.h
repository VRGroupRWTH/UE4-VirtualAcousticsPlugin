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

#ifndef IW_VANET_SERVER_IMPL
#define IW_VANET_SERVER_IMPL

#include <VANetServer.h>

#include <list>
#include <vector>
#include <string>

class VistaConnectionIP;
class VistaConnectionUpdater;
class CVANetNetworkProtocol;
class VistaMutex;

class CVANetServerImpl : public IVANetServer
{
public:
	CVANetServerImpl();
	~CVANetServerImpl();

	int Initialize( const std::string& sInterface, const int iServerPort, const int iFreePortMin = 10000, const int iFreePortMax = 11000, const int iMaxNumClients = -1 );
	int Initialize( const std::string& sInterface, const int iServerPort, const tPortList& liFreePorts, const int iMaxNumClients = -1 );
	int Finalize();
	void Reset();

	std::string GetServerAdress() const;
	int GetServerPort() const;
	IVAInterface* GetCoreInstance() const;
	void SetCoreInstance( IVAInterface* pCore );

	bool IsClientConnected() const;
	int GetNumConnectedClients() const;
	std::string GetClientHostname( const int iClientIndex ) const;

	bool AttachEventHandler( IVANetServer::IEventHandler* pHandler );
	bool DetachEventHandler( IVANetServer::IEventHandler* pHandler );

	bool AttachCoreEventHandler( VistaConnectionIP* pConnection );
	bool DetachCoreEventHandler( VistaConnectionIP* pConnection );

	void HandleClientEvent( int iEventID, VistaConnectionIP* pConnection );

protected:
	int AddNewClient( VistaConnectionIP* pTemporaryConnection );
	void RemoveClient( VistaConnectionIP* pConnection, int iStatusCode = 0 );
	void RemoveClient( int iClientID, int iStatusCode = 0 );
	int GetNextFreePort();

	void EmitEvent( const CEvent& oEvent );


protected:
	class CConnectionObserver;
	class CConnectionAccepter;
	class CEventHandler;

	struct CClientData
	{
		VistaConnectionIP*	m_pCommandChannel;
		VistaConnectionIP*	m_pHeadChannel;
		VistaConnectionIP*	m_pEventChannel;
		std::string			m_sHost;
	};

private:
	CConnectionAccepter*	m_pConnectionAccepter;
	CConnectionObserver*	m_pConnObserver;
	CEventHandler*			m_pEventHandler;

	std::vector<CClientData>	m_vecClients;

	bool					m_bInitialized;
	bool					m_bRunning;

	int						m_iMaxNumClients;

	IVAInterface*				m_pRealCore;
	CVANetNetworkProtocol*	m_pProtocol;

	std::string				m_sServerIP;
	int						m_iServerPort;

	tPortList				m_liFreePorts;

	VistaMutex*				m_pEventChannelLock;
	VistaMutex*				m_pServerChangeLock;
	VistaMutex*				m_pEventHandlerLock;

	std::list<IEventHandler*>	m_liEventHandlers;
};

#endif // IW_VANET_SERVER_IMPL
