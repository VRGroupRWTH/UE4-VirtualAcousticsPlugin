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

#ifndef IW_VANET_CLIENT_IMPL
#define IW_VANET_CLIENT_IMPL

#include <VANetClient.h>

#include <list>

class VistaConnectionIP;
class CVANetNetworkProtocol;
class IVAEventHandler;
class VistaMutex;

/**
 * Thread-Safe implementation
 */

class CVANetClientImpl : public IVANetClient
{
public:
	CVANetClientImpl();
	~CVANetClientImpl();

	IVAInterface* GetCoreInstance() const;

	bool IsConnected() const;
	std::string GetServerAddress() const;
	int Initialize( const std::string& sServerAddress, const int iServerPort, const int iHeadChannelMode = VA_HC_USE_EXISTING, const int IExceptionHandlingMode = EXC_CLIENT_THROW, const bool bBufferSynchronizedCommands = true );
	int Disconnect();
	void SendConnectionEvent( const int nId );

	void AttachEventHandler( IVAEventHandler* pCoreEventHandler );
	void DetachEventHandler( IVAEventHandler* pCoreEventHandler );

	bool AttachEventHandler( IVANetClient::IEventHandler* pHandler );
	bool DetachEventHandler( IVANetClient::IEventHandler* pHandler );

	void SetExceptionHandlingMode( const int nMode );
	int GetExceptionhandlingMode() const;

	void ProcessNetEvent( const int iEventID );
	void ProcessEventChannelError();

private:
	void Cleanup();
	void EmitEvent( const CEvent& oEvent );
	void EmitEvent( const int iID );

protected:
	class CNetworkedVACore;
	class CNetworkedVACoreThreadSafe;
	CNetworkedVACore*	m_pVACore;

	class CEventReceiver;
	CEventReceiver*		m_pEventReceiver;

	CVANetNetworkProtocol*	m_pProtocol;

	bool				m_bConnected;
	bool				m_bShutdownFlag;

	VistaConnectionIP*	m_pCommandChannel;
	VistaConnectionIP*	m_pEventChannel;
	VistaConnectionIP*	m_pHeadChannel;

	std::string			m_sServerIP;

	std::list<IEventHandler*>	m_liEventHandlers;
	VistaMutex*			m_pEventChannelLock;
};

#endif// IW_VANET_CLIENT_IMPL
