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

#ifndef IW_VANET_NETWORK_PROTOCOL
#define IW_VANET_NETWORK_PROTOCOL

#include <VA.h>
#include <VANetDefinitions.h>

#include "VANetVistaCompatibility.h"

#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

#include <list>
#include <string>

class CVAEvent;
class CVAException;
class IVAEventHandler;
class CVANetServerImpl;
class CVANetClientImpl;
class CVANetMessage;

class VistaConnectionIP;

/**
 * Basic implementation - non-threadsafe.
 *
 * This class implements the VA network protocol.
 *
 * The class defines the network package structure
 * and provides the necessary serialization and
 * deserialization.
 *
 * Instances of the class can only be used with one
 * purpose - either serializer and deserializer.
 *
 */
class CVANetNetworkProtocol
{
public:
	static const int VA_NET_PROTOCOL_VERSION_MAJOR = 2;
	static const int VA_NET_PROTOCOL_VERSION_MINOR = 0;

	/**
	 * Method identifier
	 *
	 * >>> ATTENTION!!! Never change these assignments or you end up in tears! <<<
	 */

	static const int VA_NP_INVALID = -1;

	static const int VA_NP_GET_VERSION_INFO = 0;

	// Base functions
	static const int VA_NP_GET_STATE = 100;
	static const int VA_NP_INITIALIZE = 101;
	static const int VA_NP_FINALIZE = 102;
	static const int VA_NP_RESET = 103;

	// Event handling   
	static const int VA_NP_ATTACH_EVENT_HANDLER = 201;
	static const int VA_NP_DETACH_EVENT_HANDLER = 202;

	// Module interface
	static const int VA_NP_GET_MODULES = 221;
	static const int VA_NP_CALL_MODULE = 222;
	static const int VA_NP_GET_SEARCH_PATHS = 230;
	static const int VA_NP_FIND_FILE_PATH = 231;
	static const int VA_NP_GET_CORE_CONFIGURATION = 232;
	static const int VA_NP_GET_HARDWARE_CONFIGURATION = 233;
	static const int VA_NP_GET_FILE_LIST = 234;

	// Directivities
	static const int VA_NP_CREATE_DIRECTIVITY = 300;
	static const int VA_NP_DELETE_DIRECTIVITY = 301;
	static const int VA_NP_GET_DIRECTIVITY_INFO = 302;
	static const int VA_NP_GET_DIRECTIVITY_INFOS = 303;
	static const int VA_NP_GET_DIRECTIVITY_PARAMETERS = 304;
	static const int VA_NP_SET_DIRECTIVITY_PARAMETERS = 305;
	static const int VA_NP_GET_DIRECTIVITY_NAME = 306;
	static const int VA_NP_SET_DIRECTIVITY_NAME = 307;

	// Signal sources
	static const int VA_NP_GET_SIGNALSOURCE_INFO = 600;
	static const int VA_NP_GET_SIGNALSOURCE_INFOS = 601;
	static const int VA_NP_DELETE_SIGNALSOURCE = 602;
	static const int VA_NP_GET_SIGNAL_SOURCE_PARAMETERS = 603;
	static const int VA_NP_SET_SIGNALSOURCE_PARAMETERS = 604;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_BUFFER_FROM_PARAMETERS = 610;
	static const int VA_NP_GET_SIGNAL_SOURCE_BUFFER_PLAYBACK_STATE = 611;
	static const int VA_NP_SET_SIGNAL_SOURCE_BUFFER_PLAYBACK_ACTION = 612;
	static const int VA_NP_SET_SIGNAL_SOURCE_BUFFER_PLAYBACK_POSITION = 613;
	static const int VA_NP_GET_SIGNAL_SOURCE_BUFFER_LOOPING = 614;
	static const int VA_NP_SET_SIGNAL_SOURCE_BUFFER_LOOPING = 615;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_SEQUENCER = 620;
	static const int VA_NP_ADD_SIGNAL_SOURCE_SEQUENCER_SAMPLE = 621;
	static const int VA_NP_ADD_SIGNAL_SOURCE_SEQUENCER_PLAYBACK = 622;
	static const int VA_NP_REMOVE_SIGNAL_SOURCE_SEQUENCER_SAMPLE = 623;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_NETWORK_STREAM = 630;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_ENGINE = 640;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_MACHINE = 650;

	static const int VA_NP_CREATE_SIGNAL_SOURCE_TEXT_TO_SPEECH = 660;

	// Synchronization functions
	static const int VA_NP_LOCK_UPDATE = 700;
	static const int VA_NP_UNLOCK_UPDATE = 701;
	static const int VA_NP_GET_UPDATE_LOCKED = 702;

	// Sound sources
	static const int VA_NP_GET_SOUND_SOURCE_IDS = 800;
	static const int VA_NP_CREATE_SOUND_SOURCE = 801;
	static const int VA_NP_CREATE_SOUND_SOURCE_EXPLICIT_RENDERER = 831;
	static const int VA_NP_DELETE_SOUND_SOURCE = 802;
	static const int VA_NP_SET_SOUND_SOURCE_NAME = 803;
	static const int VA_NP_GET_SOUND_SOURCE_NAME = 804;
	static const int VA_NP_SET_SOUND_SOURCE_SIGNALSOURCE = 805;
	static const int VA_NP_GET_SOUND_SOURCE_SIGNALSOURCE = 806;
	static const int VA_NP_SET_SOUND_SOURCE_AURAMODE = 807;
	static const int VA_NP_GET_SOUND_SOURCE_AURAMODE = 808;
	static const int VA_NP_SET_SOUND_SOURCE_DIRECTIVITY = 809;
	static const int VA_NP_GET_SOUND_SOURCE_DIRECTIVITY = 810;
	static const int VA_NP_SET_SOUND_SOURCE_SOUND_POWER = 811;
	static const int VA_NP_GET_SOUND_SOURCE_SOUND_POWER = 812;
	static const int VA_NP_GET_SOUND_SOURCE_MUTED = 813;
	static const int VA_NP_SET_SOUND_SOURCE_MUTED = 814;
	static const int VA_NP_SET_SOUND_SOURCE_POSE = 819;
	static const int VA_NP_GET_SOUND_SOURCE_POSE = 820;
	static const int VA_NP_SET_SOUND_SOURCE_POSITION = 815;
	static const int VA_NP_GET_SOUND_SOURCE_POSITION = 816;
	static const int VA_NP_SET_SOUND_SOURCE_ORIENTATION = 821;
	static const int VA_NP_GET_SOUND_SOURCE_ORIENTATION = 822;
	static const int VA_NP_SET_SOUND_SOURCE_ORIENTATION_VU = 817;
	static const int VA_NP_GET_SOUND_SOURCE_ORIENTATION_VU = 818;
	static const int VA_NP_SET_SOUND_SOURCE_PARAMETERS = 829;
	static const int VA_NP_GET_SOUND_SOURCE_PARAMETERS = 830;
	static const int VA_NP_SET_SOUND_SOURCE_ENABLED = 832;
	static const int VA_NP_GET_SOUND_SOURCE_ENABLED = 833;
	static const int VA_NP_GET_SOUND_SOURCE_INFO = 834;
	static const int VA_NP_GET_SOUND_SOURCE_GEOMETRY_MESH = 835;
	static const int VA_NP_SET_SOUND_SOURCE_GEOMETRY_MESH = 836;

	// Sound receivers
	static const int VA_NP_GET_SOUND_RECEIVER_IDS = 900;
	static const int VA_NP_CREATE_SOUND_RECEIVER = 901;
	static const int VA_NP_DELETE_SOUND_RECEIVER = 902;
	static const int VA_NP_GET_SOUND_RECEIVER_NAME = 903;
	static const int VA_NP_SET_SOUND_RECEIVER_NAME = 904;
	static const int VA_NP_GET_SOUND_RECEIVER_AURALIZATION_MODE = 905;
	static const int VA_NP_SET_SOUND_RECEIVER_AURALIZATION_MODE = 906;
	static const int VA_NP_GET_SOUND_RECEIVER_DIRECTIVITY = 907;
	static const int VA_NP_SET_SOUND_RECEIVER_DIRECTIVITY = 908;
	static const int VA_NP_SET_SOUND_RECEIVER_POSE = 919;
	static const int VA_NP_GET_SOUND_RECEIVER_POSE = 920;
	static const int VA_NP_SET_SOUND_RECEIVER_POSITION = 915;
	static const int VA_NP_GET_SOUND_RECEIVER_POSITION = 916;
	static const int VA_NP_SET_SOUND_RECEIVER_ORIENTATION = 921;
	static const int VA_NP_GET_SOUND_RECEIVER_ORIENTATION = 922;
	static const int VA_NP_SET_SOUND_RECEIVER_ORIENTATION_VU = 917;
	static const int VA_NP_GET_SOUND_RECEIVER_ORIENTATION_VU = 918;
	static const int VA_NP_GET_SOUND_RECEIVER_PARAMETERS = 925;
	static const int VA_NP_SET_SOUND_RECEIVER_PARAMETERS = 926;
	static const int VA_NP_SET_SOUND_RECEIVER_ENABLED = 927;
	static const int VA_NP_GET_SOUND_RECEIVER_ENABLED = 928;
	static const int VA_NP_GET_SOUND_RECEIVER_HEAD_ABOVE_TORSO_ORIENTATION = 929;
	static const int VA_NP_SET_SOUND_RECEIVER_HEAD_ABOVE_TORSO_ORIENTATION = 930;
	static const int VA_NP_GET_SOUND_RECEIVER_REAL_WORLD_POSITION_ORIENTATION_VU = 931;
	static const int VA_NP_SET_SOUND_RECEIVER_REAL_WORLD_POSITION_ORIENTATION_VU = 932;
	static const int VA_NP_GET_SOUND_RECEIVER_GEOMETRY_MESH = 933;
	static const int VA_NP_SET_SOUND_RECEIVER_GEOMETRY_MESH = 934;
	static const int VA_NP_GET_SOUND_RECEIVER_REAL_WORLD_POSE = 935;
	static const int VA_NP_SET_SOUND_RECEIVER_REAL_WORLD_POSE = 936;
	static const int VA_NP_GET_SOUND_RECEIVER_REAL_WORLD_TORSO_ORIENTATION = 937;
	static const int VA_NP_SET_SOUND_RECEIVER_REAL_WORLD_TORSO_ORIENTATION = 938;
	static const int VA_NP_GET_SOUND_RECEIVER_INFO = 939;
	static const int VA_NP_CREATE_SOUND_RECEIVER_EXPLICIT_RENDERER = 940;



	// Scene
	static const int VA_NP_SCENE_CREATE = 1000;
	static const int VA_NP_GET_SCENE_IDS = 1001;
	static const int VA_NP_GET_SCENE_INFO = 1002;
	static const int VA_NP_GET_SCENE_NAME = 1003;
	static const int VA_NP_SET_SCENE_NAME = 1004;
	static const int VA_NP_GET_SCENE_ENABLED = 1005;
	static const int VA_NP_SET_SCENE_ENABLED = 1006;

	// Portals
	static const int VA_NP_CREATE_SOUND_PORTAL = 1600;
	static const int VA_NP_GET_SOUND_PORTAL_IDS = 1601;
	static const int VA_NP_GET_SOUND_PORTAL_NAME = 1602;
	static const int VA_NP_SET_SOUND_PORTAL_NAME = 1603;
	static const int VA_NP_GET_SOUND_PORTAL_PARAMETERS = 1604;
	static const int VA_NP_SET_SOUND_PORTAL_PARAMETERS = 1605;
	static const int VA_NP_GET_SOUND_PORTAL_POSITION = 1606;
	static const int VA_NP_SET_SOUND_PORTAL_POSITION = 1607;
	static const int VA_NP_GET_SOUND_PORTAL_ORIENTATION = 1608;
	static const int VA_NP_SET_SOUND_PORTAL_ORIENTATION = 1609;
	static const int VA_NP_GET_SOUND_PORTAL_ENABLED = 1610;
	static const int VA_NP_SET_SOUND_PORTAL_ENABLED = 1611;
	static const int VA_NP_GET_SOUND_PORTAL_MATERIAL = 1612;
	static const int VA_NP_SET_SOUND_PORTAL_MATERIAL = 1613;
	static const int VA_NP_GET_SOUND_PORTAL_NEXT_SOUND_PORTAL = 1614;
	static const int VA_NP_SET_SOUND_PORTAL_NEXT_SOUND_PORTAL = 1615;
	static const int VA_NP_GET_SOUND_PORTAL_SOUND_SOURCE = 1616;
	static const int VA_NP_SET_SOUND_PORTAL_SOUND_SOURCE = 1617;
	static const int VA_NP_GET_SOUND_PORTAL_SOUND_RECEIVER = 1618;
	static const int VA_NP_SET_SOUND_PORTAL_SOUND_RECEIVER = 1619;
	static const int VA_NP_GET_SOUND_PORTAL_INFO = 1620;

	// Geometry meshes
	static const int VA_NP_CREATE_GEOMETRY_MESH = 1700;
	static const int VA_NP_CREATE_GEOMETRY_MESH_FROM_PARAMETERS = 1701;
	static const int VA_NP_DELETE_GEOMETRY_MESH = 1702;
	static const int VA_NP_GET_GEOMETRY_MESH = 1703;
	static const int VA_NP_GET_GEOMETRY_MESH_IDS = 1704;
	static const int VA_NP_GET_GEOMETRY_MESH_PARAMETERS = 1705;
	static const int VA_NP_SET_GEOMETRY_MESH_PARAMETERS = 1706;
	static const int VA_NP_GET_GEOMETRY_MESH_NAME = 1707;
	static const int VA_NP_SET_GEOMETRY_MESH_NAME = 1708;
	static const int VA_NP_GET_GEOMETRY_MESH_ENABLED = 1709;
	static const int VA_NP_SET_GEOMETRY_MESH_ENABLED = 1710;

	// Acoustic materials
	static const int VA_NP_CREATE_ACOUSTIC_MATERIAL = 1400;
	static const int VA_NP_CREATE_ACOUSTIC_MATERIAL_FROM_PARAMETERS = 1401;
	static const int VA_NP_DELETE_ACOUSTIC_MATERIAL = 1402;
	static const int VA_NP_GET_ACOUSTIC_MATERIAL_INFO = 1403;
	static const int VA_NP_GET_ACOUSTIC_MATERIAL_INFOS = 1405;
	static const int VA_NP_GET_ACOUSTIC_MATERIAL_NAME = 1406;
	static const int VA_NP_SET_ACOUSTIC_MATERIAL_NAME = 1407;
	static const int VA_NP_GET_ACOUSTIC_MATERIAL_PARAMETERS = 1408;
	static const int VA_NP_SET_ACOUSTIC_MATERIAL_PARAMETERS = 1409;

	// Medium
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_SOUND_SPEED = 1500;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_SOUND_SPEED = 1501;
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_TEMPERATURE = 1502;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_TEMPERATURE = 1503;
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_STATIC_PRESSURE = 1504;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_STATIC_PRESSURE = 1505;
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_RELATIVE_HUMIDITY = 1506;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_RELATIVE_HUMIDITY = 1507;
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_SHIFT_SPEED = 1508;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_SHIFT_SPEED = 1509;
	static const int VA_NP_GET_HOMOGENEOUS_MEDIUM_PARAMETERS = 1510;
	static const int VA_NP_SET_HOMOGENEOUS_MEDIUM_PARAMETERS = 1511;

	// Control
	static const int VA_NP_GET_INPUT_GAIN = 1100;
	static const int VA_NP_SET_INPUT_GAIN = 1101;
	static const int VA_NP_GET_INPUT_MUTED = 1102;
	static const int VA_NP_SET_INPUT_MUTED = 1103;
	static const int VA_NP_GET_OUTPUT_GAIN = 1104;
	static const int VA_NP_SET_OUTPUT_GAIN = 1105;
	static const int VA_NP_GET_OUTPUT_MUTED = 1106;
	static const int VA_NP_SET_OUTPUT_MUTED = 1107;
	static const int VA_NP_GET_GLOBAL_AURALIZATION_MODE = 1108;
	static const int VA_NP_SET_GLOBAL_AURALIZATION_MODE = 1109;
	static const int VA_NP_GET_ACTIVE_SOUND_RECEIVER = 1110;
	static const int VA_NP_SET_ACTIVE_SOUND_RECEIVER = 1111;
	static const int VA_NP_GET_CORE_CLOCK = 1112;
	static const int VA_NP_SET_CORE_CLOCK = 1113;
	static const int VA_NP_SUBSTITUTE_MACROS = 1114;
	static const int VA_NP_GET_ACTIVE_SOUND_RECEIVER_EXPLICIT_RENDERER = 1115;
	static const int VA_NP_SET_ACTIVE_SOUND_RECEIVER_EXPLICIT_RENDERER = 1116;

	// Renderers
	static const int VA_NP_RENDERER_GET_INFOS = 1200;
	static const int VA_NP_RENDERER_SET_MUTED = 1201;
	static const int VA_NP_RENDERER_GET_MUTED = 1202;
	static const int VA_NP_RENDERER_GET_GAIN = 1203;
	static const int VA_NP_RENDERER_SET_GAIN = 1204;
	static const int VA_NP_RENDERER_GET_AURALIZATION_MODE = 1205;
	static const int VA_NP_RENDERER_SET_AURALIZATION_MODE = 1206;
	static const int VA_NP_RENDERER_GET_PARAMETERS = 1207;
	static const int VA_NP_RENDERER_SET_PARAMETERS = 1208;

	// Reproductions
	static const int VA_NP_REPRODUCTION_GET_INFOS = 1300;
	static const int VA_NP_REPRODUCTION_SET_MUTED = 1301;
	static const int VA_NP_REPRODUCTION_GET_MUTED = 1302;
	static const int VA_NP_REPRODUCTION_GET_GAIN = 1303;
	static const int VA_NP_REPRODUCTION_SET_GAIN = 1304;
	static const int VA_NP_REPRODUCTION_GET_PARAMETERS = 1305;
	static const int VA_NP_REPRODUCTION_SET_PARAMETERS = 1306;

	// Internally used literals
	static const int VA_NP_SPECIAL_BUFFERED_MESSAGE = 42001;
	static const int VA_NP_SPECIAL_CLIENT_EVENT = 42002;

	static const int VA_NP_SPECIAL_ANSWER = 42003;
	static const int VA_NP_SPECIAL_EXCEPTION = 42004;
	static const int VA_NP_SPECIAL_NO_EXCEPTION = 42005;


	static const int VA_NET_CLIENT_CLOSE = -1001;
	static const int VA_NET_SERVER_CLOSE = -1002;
	static const int VA_NET_SERVER_RESET = -1003;
	static const int VA_NET_SERVER_DISCONNECT = -1004;

	static const int VA_EVENT_FAIL = 0;
	static const int VA_EVENT_CORE_EVENT = 1;
	static const int VA_EVENT_PROGRESS_UPDATE = 2;
	static const int VA_EVENT_NET_EVENT = 3;

	static const int VA_NO_EXCEPTION_INDICATOR = -31415926;

public:
	CVANetNetworkProtocol();
	~CVANetNetworkProtocol();

	bool InitializeAsServer( CVANetServerImpl* pServer );
	void SetRealVACore( IVAInterface* pRealCore );

	bool InitializeAsClient( CVANetClientImpl* pServer, VistaConnectionIP* m_pCommandChannel, VistaConnectionIP* m_pHeadChannel, const int iExceptionhandlingmode, const bool m_bBufferSynchronisedModification );

	int ClientProcessEventMessageFromServer( VistaConnectionIP* pConnection, std::list<IVAEventHandler*> liCoreEventHandler );
	void ServerCallFunctionByMessageType( const int nMessageType, VistaConnectionIP* pConnection );
	bool ProcessMessageFromClient( VistaConnectionIP* pAnswerConnection );
	void ServerPrepareEventMessage( const CVAEvent* pEvent, CVANetMessage* pMessage );

	// for client side only
	void SetExceptionHandlingMode( const int nMode );
	int GetExceptionHandlingMode() const;

	bool GetBufferSynchronisedModification() const;
	void SetBufferSynchronisedModifications( const bool bSet );

	void HandleConnectionClose( VistaConnectionIP* pConnection );

private:
	enum MESSAGE_MODE
	{
		// Command Message will return an answer
		// all exceptions that occur will always be transmitted over network
		MESSAGE_WITH_ANSWER,

		// Command Message does not an return answer (no return value)
		// all exceptions that occur will always be transmitted over network
		MESSAGE_ENFORCED_EXCEPTION,

		// Command Message does not an return answer (no return value)
		// exceptions that occur will only be transmitted if the user requests
		// it on client side (configurable)
		MESSAGE_CONDITIONAL_EXCEPTION,

		// Command Message does not return an answer (no return value),
		// and also allows buffering between Begin- and End-Synchronised-Modification 
		// (if the appropriate buffering flag is set on the client).
		// All exceptions will only be transmitted if the user requests
		// it on client-side (configurable), or after the EndSynchronisedModification call
		MESSAGE_ALLOWS_BUFFERING,

		// Note: Any function that has a return value, cannot be buffered and
		//       any exceptions will be transmitted automatically.
	};

	CVANetMessage* ClientInitMessage( const int nCommanId, MESSAGE_MODE eMessageMode );
	void ClientSendCommand( CVANetMessage* pMessage );

	CVANetMessage* ServerGetMessage();
	void ServerSendAnswer( CVANetMessage* pMessage );

	void ClientPrepareMessageBuffering();
	int ClientEndMessageBuffering();

public:

	/**
	 * Client/Server actions for individual requests
	 */

	void ClientGetVersionInfo( CVAVersionInfo* pVersionInfo );
	void ServerGetVersionInfo();
	void ClientAttachEventHandler();
	void ServerAttachEventHandler( VistaConnectionIP* pConn );
	void ClientDetachEventHandler();
	void ServerDetachEventHandler( VistaConnectionIP* pConn );

	int ClientGetState();
	void ServerGetState();

	void ClientInitialize();
	void ServerInitialize();

	void ClientFinalize();
	void ServerFinalize();

	void ClientReset();
	void ServerReset();

	void ClientGetModules( std::vector< CVAModuleInfo >& m_viModuleInfos );
	void ServerGetModules();

	CVAStruct ClientGetSearchPaths();
	void ServerGetSearchPaths();

	std::string ClientFindFilePath( const std::string& sFilePath );
	void ServerFindFilePath();

	CVAStruct ClientGetCoreConfiguration( const bool bFilterEnabled = true );
	void ServerGetCoreConfiguration();
	CVAStruct ClientGetHardwareConfiguration();
	void ServerGetHardwareConfiguration();
	CVAStruct ClientGetFileList( const bool bRecursive = true, const std::string& sSuffixFilter = "*" );
	void ServerGetFileList();

	CVAStruct ClientCallModule( const std::string& sModuleName, const CVAStruct& oArgs );
	void ServerCallModule();

	int ClientCreateDirectivityFromParameters( const CVAStruct& oParams, const std::string& sName = "" );
	void ServerCreateDirectivityFromParameters();

	bool ClientDeleteDirectivity( const int iID );
	void ServerDeleteDirectivity();

	CVADirectivityInfo ClientGetDirectivityInfo( const int iID );
	void ServerGetDirectivityInfo();

	void ClientGetDirectivityInfos( std::vector<CVADirectivityInfo>& vdiDest );
	void ServerGetDirectivityInfos();

	void ClientSetDirectivityName( const int iID, const std::string& sName );
	void ServerSetDirectivityName();
	std::string ClientGetDirectivityName( const int iID );
	void ServerGetDirectivityName();

	CVAStruct ClientGetDirectivityParameters( const int iID, const CVAStruct& oParams );
	void ServerGetDirectivityParameters();
	void ClientSetDirectivityParameters( const int iID, const CVAStruct& oParams );
	void ServerSetDirectivityParameters();


	std::string ClientCreateSignalSourceBufferFromParameters( const CVAStruct& oParams, const std::string& sName = "" );
	void ServerCreateSignalSourceBufferFromParameters();

	std::string ClientCreateTextToSpeechSignalSource( const std::string& sName = "" );
	void ServerCreateTextToSpeechSignalSource();

	std::string ClientCreateSequencerSignalSource( const std::string& sName = "" );
	void ServerCreateSequencerSignalSource();

	std::string ClientCreateNetworkStreamSignalSource( const std::string& sInterface, int iPort, const std::string& sName = "" );
	void ServerCreateNetworkStreamSignalSource();

	std::string ClientCreateEngineSignalSource( const CVAStruct& oParams, const std::string& sName = "" );
	void ServerCreateEngineSignalSource();

	std::string ClientCreateSignalSourceMachine( const CVAStruct& oParams, const std::string& sName = "" );
	void ServerCreateSignalSourceMachine();

	bool ClientDeleteSignalSource( const std::string& sID );
	void ServerDeleteSignalSource();

	CVASignalSourceInfo ClientGetSignalSourceInfo( const std::string& sSignalSourceID );
	void ServerGetSignalSourceInfo();
	void ClientGetSignalSourceInfos( std::vector< CVASignalSourceInfo >& vssiDest );
	void ServerGetSignalSourceInfos();

	int ClientGetSignalSourceBufferPlaybackState( const std::string& sSignalSourceID );
	void ServerGetSignalSourceBufferPlaybackState();

	void ClientSetSignalSourceBufferPlaybackAction( const std::string& sSignalSourceID, const int iPlayState );
	void ServerSetSignalSourceBufferPlaybackAction();

	void ClientSetSignalSourceBufferPlaybackPosition( const std::string& sSignalSourceID, const double dPlaybackPosition );
	void ServerSetAudiofileSignalSourcePlaybackPosition();

	bool ClientGetSignalSourceBufferLooping( const std::string& );
	void ServerGetSignalSourceBufferLooping();
	void ClientSetSignalSourceBufferLooping( const std::string&, const bool bLoopingEnabled );
	void ServerSetSignalSourceBufferLooping();

	void ClientSetSignalSourceParameters( const std::string& sSignalSourceID, const CVAStruct& oParams );
	void ServerSetSignalSourceParameters();
	CVAStruct ClientGetSignalSourceParameters( const std::string& sSignalSourceID, const CVAStruct& oParams );
	void ServerGetSignalSourceParameters();

	int ClientAddSignalSourceSequencerSample( const std::string& sSignalSourceID, const CVAStruct& oArgs );
	void ServerAddSignalSourceSequencerSample();
	int ClientAddSignalSourceSequencerPlayback( const std::string& sSignalSourceID, const int iSoundID, const int iFlags, const double dTimeCode );
	void ServerAddSignalSourceSequencerPlayback();
	void ClientRemoveSignalSourceSequencerSample( const std::string& sSignalSourceID, const int iSoundID );
	void ServerRemoveSignalSourceSequencerSample();


	void ClientLockUpdate();
	void ServerLockUpdate();

	int ClientUnlockUpdate();
	void ServerUnlockUpdate();

	bool ClientGetUpdateLocked();
	void ServerGetUpdateLocked();


	void ClientGetSoundSourceIDs( std::vector<  int >& vSoundSourceIDs );
	void ServerGetSoundSourceIDs();

	int ClientCreateSoundSource( const std::string& sName );
	void ServerCreateSoundSource();

	int ClientCreateSoundSourceExplicitRenderer( const std::string& sRendererID, const std::string& sName );
	void ServerCreateSoundSourceExplicitRenderer();

	int ClientDeleteSoundSource( const int iID );
	void ServerDeleteSoundSource();

	void ClientSetSoundSourceEnabled( const int iID, const bool bEnabled );
	void ServerSetSoundSourceEnabled();
	bool ClientGetSoundSourceEnabled( const int iID );
	void ServerGetSoundSourceEnabled();

	CVASoundSourceInfo ClientGetSoundSourceInfo( const int iID );
	void ServerGetSoundSourceInfo();

	std::string ClientGetSoundSourceName( const int iSoundSourceID );
	void ServerGetSoundSourceName();
	void ClientSetSoundSourceName( const int iSoundSourceID, const std::string& sName );
	void ServerSetSoundSourceName();

	std::string ClientGetSoundSourceSignalSource( const int iSoundSourceID );
	void ServerGetSoundSourceSignalSource();
	void ClientSetSoundSourceSignalSource( const int iSoundSourceIDconst, const std::string& sSignalSourceID );
	void ServerSetSoundSourceSignalSource();

	int ClientGetSoundSourceAuralizationMode( const int iSoundSourceID );
	void ServerGetSoundSourceAuralizationMode();
	void ClientSetSoundSourceAuralizationMode( const int iSoundSourceID, const int iAuralizationMode );
	void ServerSetSoundSourceAuralizationMode();

	void ClientSetSoundSourceParameters( const int iID, const CVAStruct& oParams );
	void ServerSetSoundSourceParameters();
	CVAStruct ClientGetSoundSourceParameters( const int iID, const CVAStruct& oArgs );
	void ServerGetSoundSourceParameters();

	int ClientGetSoundSourceDirectivity( const int iSoundSourceID );
	void ServerGetSoundSourceDirectivity();
	void ClientSetSoundSourceDirectivity( const int iSoundSourceID, const int iDirectivityID );
	void ServerSetSoundSourceDirectivity();

	double ClientGetSoundSourceSoundPower( const int iSoundSourceID );
	void ServerGetSoundSourceSoundPower();
	void ClientSetSoundSourceSoundPower( const int iSoundSourceID, const double dPower );
	void ServerSetSoundSourceSoundPower();

	bool ClientGetSoundSourceMuted( const int iSoundSourceID );
	void ServerGetSoundSourceMuted();
	void ClientSetSoundSourceMuted( const int iSoundSourceID, const bool bMuted );
	void ServerSetSoundSourceMuted();

	void ClientGetSoundSourcePose( const int iSoundSourceID, VAVec3& vPos, VAQuat& qOrient );
	void ServerGetSoundSourcePose();
	void ClientSetSoundSourcePose( const int iSoundSourceID, const VAVec3& vPos, const VAQuat& qOrient );
	void ServerSetSoundSourcePose();

	VAVec3 ClientGetSoundSourcePosition( const int iSoundSourceID );
	void ServerGetSoundSourcePosition();
	void ClientSetSoundSourcePosition( const int iID, const VAVec3& v3Pos );
	void ServerSetSoundSourcePosition();

	VAQuat ClientGetSoundSourceOrientation( const int iSoundSourceID );
	void ServerGetSoundSourceOrientation();
	void ClientSetSoundSourceOrientation( const int iID, const VAQuat& qOrient );
	void ServerSetSoundSourceOrientation();

	void ClientGetSoundSourceOrientationVU( const int iSoundSourceID, VAVec3& v3View, VAVec3& v3Up );
	void ServerGetSoundSourceOrientationVU();
	void ClientSetSoundSourceOrientationVU( const int iID, const VAVec3& v3View, const VAVec3& v3Up );
	void ServerSetSoundSourceOrientationVU();

	int ClientGetSoundSourceGeometryMesh( const int iID );
	void ServerGetSoundSourceGeometryMesh();
	void ClientSetSoundSourceGeometryMesh( const int iSoundSourceID, const int iGeometryMeshID );
	void ServerSetSoundSourceGeometryMesh();


	void ClientGetSoundReceiverIDs( std::vector<int>& viIDs );
	void ServerGetSoundReceiverIDs();

	int ClientCreateSoundReceiver( const std::string& sName );
	void ServerCreateSoundReceiver();
	int ClientCreateSoundReceiverExplicitRenderer( const std::string& sRendererID, const std::string& sName );
	void ServerCreateSoundReceiverExplicitRenderer();

	int ClientDeleteSoundReceiver( const int iID );
	void ServerDeleteSoundReceiver();
	CVASoundReceiverInfo ClientGetSoundReceiverInfo( const int iID );
	void ServerGetSoundReceiverInfo();

	void ClientSetSoundReceiverEnabled( const int, const bool );
	void ServerSetSoundReceiverEnabled();
	bool ClientGetSoundReceiverEnabled( int );
	void ServerGetSoundReceiverEnabled();

	void ClientSetActiveSoundReceiverExplicitRenderer( int, const std::string& );
	void ServerSetActiveSoundReceiverExplicitRenderer();
	int ClientGetActiveSoundReceiverExplicitRenderer( const std::string& );
	void ServerGetActiveSoundReceiverExplicitRenderer();

	std::string ClientGetSoundReceiverName( const int iID );
	void ServerGetSoundReceiverName();
	void ClientSetSoundReceiverName( int iID, const std::string& sName );
	void ServerSetSoundReceiverName();

	int ClientGetSoundReceiverAuralizationMode( const int iID );
	void ServerGetSoundReceiverAuralizationMode();
	void ClientSetSoundReceiverAuralizationMode( const int iID, const int iAuralizationMode );
	void ServerSetSoundReceiverAuralizationMode();

	void ClientSetSoundReceiverParameters( const int iID, const CVAStruct& oParams );
	void ServerSetSoundReceiverParameters();
	CVAStruct ClientGetSoundReceiverParameters( const int iID, const CVAStruct& oArgs );
	void ServerGetSoundReceiverParameters();

	int ClientGetSoundReceiverDirectivity( const int iSoundReceiverID );
	void ServerGetSoundReceiverDirectivity();
	void ClientSetSoundReceiverDirectivity( const int iSoundReceiverID, const int iDirectivityID );
	void ServerSetSoundReceiverDirectivity();

	void ClientGetSoundReceiverPose( const int iID, VAVec3& v3Pos, VAQuat& qOrient );
	void ServerGetSoundReceiverPose();
	void ClientSetSoundReceiverPose( const int iID, const VAVec3& v3Pos, const VAQuat& qOrient );
	void ServerSetSoundReceiverPose();

	VAVec3 ClientGetSoundReceiverPosition( const int iID );
	void ServerGetSoundReceiverPosition();
	void ClientSetSoundReceiverPosition( const int iID, const VAVec3& v3Pos );
	void ServerSetSoundReceiverPosition();

	VAQuat ClientGetSoundReceiverOrientation( const int iID );
	void ServerGetSoundReceiverOrientation();
	void ClientSetSoundReceiverOrientation( const int iID, const VAQuat& qOrient );
	void ServerSetSoundReceiverOrientation();

	void ClientGetSoundReceiverOrientationVU( const int iID, VAVec3& v3View, VAVec3& v3Up );
	void ServerGetSoundReceiverOrientationVU();
	void ClientSetSoundReceiverOrientationVU( const int iID, const VAVec3& v3View, const VAVec3& v3Up );
	void ServerSetSoundReceiverOrientationVU();

	VAQuat ClientGetSoundReceiverHeadAboveTorsoOrientation( const int iID );
	void ServerGetSoundReceiverHeadAboveTorsoOrientation();
	void ClientSetSoundReceiverHeadAboveTorsoOrientation( const int iID, const VAQuat& qOrient );
	void ServerSetSoundReceiverHeadAboveTorsoOrientation();

	int ClientGetSoundReceiverGeometryMesh( const int iID );
	void ServerGetSoundReceiverGeometryMesh();
	void ClientSetSoundReceiverGeometryMesh( const int iSoundReceiverID, const int iGeometryMeshID );
	void ServerSetSoundReceiverGeometryMesh();

	void ClientGetSoundReceiverRealWorldPose( const int iID, VAVec3& v3Pos, VAQuat& qOrient );
	void ServerGetSoundReceiverRealWorldPose();
	void ClientSetSoundReceiverRealWorldPose( const int iID, const VAVec3& v3Pos, const VAQuat& qOrient );
	void ServerSetSoundReceiverRealWorldPose();

	void ClientGetSoundReceiverRealWorldPositionOrientationVU( const int iID, VAVec3& v3Pos, VAVec3& v3View, VAVec3& v3Up );
	void ServerGetSoundReceiverRealWorldPositionOrientationVU();
	void ClientSetSoundReceiverRealWorldPositionOrientationVU( const int iID, const VAVec3& v3Pos, const VAVec3& v3View, const VAVec3& v3Up );
	void ServerSetSoundReceiverRealWorldPositionOrientationVU();

	VAQuat ClientGetSoundReceiverRealWorldTorsoOrientation( const int iID );
	void ServerGetSoundReceiverRealWorldTorsoOrientation();
	void ClientSetSoundReceiverRealWorldTorsoOrientation( const int iID, const VAQuat& qOrient );
	void ServerSetSoundReceiverRealWorldTorsoOrientation();


	std::string ClientCreateScene( const CVAStruct& oParams, const std::string& sName );
	void ServerCreateScene();

	void ClientGetSceneIDs( std::vector< std::string >& vsIDs );
	void ServerGetSceneIDs();

	CVASceneInfo ClientGetSceneInfo( const std::string& sID );
	void ServerGetSceneInfo();

	std::string ClientGetSceneName( const std::string& sID );
	void ServerGetSceneName();
	void ClientSetSceneName( const std::string& sID, const std::string& sName );
	void ServerSetSceneName();

	bool ClientGetSceneEnabled( const std::string& sID );
	void ServerGetSceneEnabled();
	void ClientSetSceneEnabled( const std::string& sID, const bool bEnabled );
	void ServerSetSceneEnabled();


	int ClientCreateSoundPortal( const std::string& sName );
	void ServerCreateSoundPortal();

	void ClientGetSoundPortalIDs( std::vector<int>& vPortalIDs );
	void ServerGetSoundPortalIDs();

	CVASoundPortalInfo ClientGetSoundPortalInfo( const int iID );
	void ServerGetSoundPortalInfo();

	std::string ClientGetSoundPortalName( const int iPortalID );
	void ServerGetSoundPortalName();
	void ClientSetSoundPortalName( const int iPortalID, const std::string& sName );
	void ServerSetSoundPortalName();

	int ClientGetSoundPortalMaterial( const int iSoundPortalID );
	void ServerGetSoundPortalMaterial();
	void ClientSetSoundPortalMaterial( const int iSoundPortalID, const int iMaterialID );
	void ServerSetSoundPortalMaterial();

	int ClientGetSoundPortalNextPortal( const int iSoundPortalID );
	void ServerGetSoundPortalNextPortal();
	void ClientSetSoundPortalNextPortal( const int iSoundPortalID, const int iNextSoundPortalID );
	void ServerSetSoundPortalNextPortal();

	int ClientGetSoundPortalSoundReceiver( const int iSoundPortalID );
	void ServerGetSoundPortalSoundReceiver();
	void ClientSetSoundPortalSoundReceiver( const int iSoundPortalID, const int iSoundReceiverID );
	void ServerSetSoundPortalSoundReceiver();

	int ClientGetSoundPortalSoundSource( const int iSoundPortalID );
	void ServerGetSoundPortalSoundSource();
	void ClientSetSoundPortalSoundSource( const int iSoundPortalID, const int iSoundSourceID );
	void ServerSetSoundPortalSoundSource();

	VAVec3 ClientGetSoundPortalPosition( const int iSoundPortalID );
	void ServerGetSoundPortalPosition();
	void ClientSetSoundPortalPosition( const int iSoundPortalID, const VAVec3& vPos );
	void ServerSetSoundPortalPosition();

	VAQuat ClientGetSoundPortalOrientation( const int iSoundPortalID );
	void ServerGetSoundPortalOrientation();
	void ClientSetSoundPortalOrientation( const int iSoundPortalID, const VAQuat& qOrient );
	void ServerSetSoundPortalOrientation();

	bool ClientGetSoundPortalEnabled( const int iSoundPortalID );
	void ServerGetSoundPortalEnabled();
	void ClientSetSoundPortalEnabled( const int iSoundPortalID, const bool bEnabled );
	void ServerSetSoundPortalEnabled();

	CVAStruct ClientGetSoundPortalParameters( const int iPortalID, const CVAStruct& oArgs );
	void ServerGetSoundPortalParameters();
	void ClientSetSoundPortalParameters( const int iPortalID, const CVAStruct& oParams );
	void ServerSetSoundPortalParameters();


	int ClientCreateGeometryMesh( const CVAGeometryMesh& oMesh, const std::string& sName );
	void ServerCreateGeometryMesh();
	int ClientCreateGeometryMeshFromParameters( const CVAStruct& oParams, const std::string& sName );
	void ServerCreateGeometryMeshFromParameters();
	bool ClientDeleteGeometryMesh( const int iID );
	void ServerDeleteGeometryMesh();
	CVAGeometryMesh ClientGetGeometryMesh( const int iID );
	void ServerGetGeometryMesh();
	void ClientGetGeometryMeshIDs( std::vector< int >& viIDs );
	void ServerGetGeometryMeshIDs();
	void ClientSetGeometryMeshName( const int iID, const std::string& sName );
	void ServerClientSetGeometryMeshName();
	std::string ClientGetGeometryMeshName( const int iID );
	void ServerGetGeometryMeshName();
	void ClientSetGeometryMeshParameters( const int iID, const CVAStruct& oParams );
	void ServerSetGeometryMeshParameters();
	CVAStruct ClientGetGeometryMeshParameters( const int iID, const CVAStruct& oParams );
	void ServerGetGeometryMeshParameters();
	void ClientSetGeometryMeshEnabled( const int iID, const bool bEnabled );
	void ServerSetGeometryMeshEnabled();
	bool ClientGetGeometryMeshEnabled( const int iID );
	void ServerGetGeometryMeshEnabled();


	double ClientGetCoreClock();
	void ServerGetCoreClock();
	void ClientSetCoreClock( const double dSeconds );
	void ServerSetCoreClock();


	double ClientGetInputGain();
	void ServerGetInputGain();

	void ClientSetInputGain( double dGain );
	void ServerSetInputGain();

	bool ClientGetInputMuted();
	void ServerGetInputMuted();

	void ClientSetInputMuted( bool bMuted );
	void ServerSetInputMuted();

	double ClientGetOutputGain();
	void ServerGetOutputGain();

	void ClientSetOutputGain( double dGain );
	void ServerSetOutputGain();

	bool ClientGetOutputMuted();
	void ServerGetOutputMuted();

	void ClientSetOutputMuted( const bool bMuted );
	void ServerSetOutputMuted();

	int ClientGetGlobalAuralizationMode();
	void ServerGetGlobalAuralizationMode();

	void ClientSetGlobalAuralizationMode( int iAuralizationMode );
	void ServerSetGlobalAuralizationMode();

	int ClientGetActiveSoundReceiver();
	void ServerGetActiveSoundReceiver();

	void ClientSetActiveSoundReceiver( const int iID );
	void ServerSetActiveSoundReceiver();

	std::string ClientSubstituteMacros( const std::string& sStr );
	void ServerSubstituteMacros();

	void ClientSetRenderingModuleGain( const std::string&, const double );
	void ServerSetRenderingModuleGain();

	void ClientSetRenderingModuleMuted( const std::string&, const bool );
	void ServerSetRenderingModuleMuted();

	bool ClientGetRenderingModuleMuted( const std::string& );
	void ServerGetRenderingModuleMuted();

	double ClientGetRenderingModuleGain( const std::string& );
	void ServerGetRenderingModuleGain();

	void ClientSetRenderingModuleParameters( const std::string&, const CVAStruct& );
	void ServerSetRenderingModuleParameters();

	CVAStruct ClientGetRenderingModuleParameters( const std::string&, const CVAStruct& );
	void ServerGetRenderingModuleParameters();

	void ClientSetRenderingModuleAuralizationMode( const std::string&, const int );
	void ServerSetRenderingModuleAuralizationMode();

	int ClientGetRenderingModuleAuralizationMode( const std::string& );
	void ServerGetRenderingModuleAuralizationMode();

	void ClientGetRenderingModuleInfos( std::vector< CVAAudioRendererInfo >&, bool );
	void ServerGetRenderingModuleInfos();

	void ClientSetReproductionModuleGain( const std::string&, const double );
	void ServerSetReproductionModuleGain();

	void ClientSetReproductionModuleMuted( const std::string&, const bool );
	void ServerSetReproductionModuleMuted();

	bool ClientGetReproductionModuleMuted( const std::string& );
	void ServerGetReproductionModuleMuted();

	double ClientGetReproductionModuleGain( const std::string& );
	void ServerGetReproductionModuleGain();

	void ClientGetReproductionModuleInfos( std::vector< CVAAudioReproductionInfo >&, const bool );
	void ServerGetReproductionModuleInfos();

	void ClientSetReproductionModuleParameters( const std::string&, const CVAStruct& );
	void ServerSetReproductionModuleParameters();

	CVAStruct ClientGetReproductionModuleParameters( const std::string&, const CVAStruct& );
	void ServerGetReproductionModuleParameters();


	int ClientCreateAcousticMaterial( const CVAAcousticMaterial& oMaterial, const std::string& sName );
	void ServerCreateAcousticMaterial();
	int ClientCreateAcousticMaterialFromParameters( const CVAStruct& oParams, const std::string& sName );
	void ServerCreateAcousticMaterialFromParameters();
	bool ClientDeleteAcousticMaterial( const int iID );
	void ServerDeleteAcousticMaterial();

	CVAAcousticMaterial ClientGetAcousticMaterialInfo( const int iID );
	void ServerGetAcousticMaterialInfo();

	void ClientGetAcousticMaterialInfos( std::vector< CVAAcousticMaterial >& voDest );
	void ServerGetAcousticMaterialInfos();

	std::string ClientGetAcousticMaterialName( const int iID );
	void ServerGetAcousticMaterialName();
	void ClientSetAcousticMaterialName( const int iID, const std::string& sName );
	void ServerSetAcousticMaterialName();

	CVAStruct ClientGetAcousticMaterialParameters( const int iID, const CVAStruct& oParams );
	void ServerGetAcousticMaterialParameters();
	void ClientSetAcousticMaterialParameters( const int iID, const CVAStruct& oParams );
	void ServerSetAcousticMaterialParameters();


	void ClientSetHomogeneousMediumSoundSpeed( const double dSoundSpeed );
	void ServerSetHomogeneousMediumSoundSpeed();
	double ClientGetHomogeneousMediumSoundSpeed();
	void ServerGetHomogeneousMediumSoundSpeed();

	void ClientSetHomogeneousMediumTemperature( const double dDegreesCentigrade );
	void ServerSetHomogeneousMediumTemperature();
	double ClientGetHomogeneousMediumTemperature();
	void ServerGetHomogeneousMediumTemperature();

	void ClientSetHomogeneousMediumStaticPressure( const double dPressurePascal );
	void ServerSetHomogeneousMediumStaticPressure();
	double ClientGetHomogeneousMediumStaticPressure();
	void ServerGetHomogeneousMediumStaticPressure();

	void ClientSetHomogeneousMediumRelativeHumidity( const double dRelativeHumidityPercent );
	void ServerSetHomogeneousMediumRelativeHumidity();
	double ClientGetHomogeneousMediumRelativeHumidity();
	void ServerGetHomogeneousMediumRelativeHumidity();

	void ClientSetHomogeneousMediumShiftSpeed( const VAVec3& v3TranslationSpeed );
	void ServerSetHomogeneousMediumShiftSpeed();
	VAVec3 ClientGetHomogeneousMediumShiftSpeed();
	void ServerGetHomogeneousMediumShiftSpeed();

	void ClientSetHomogeneousMediumParameters( const CVAStruct& oParams );
	void ServerSetHomogeneousMediumParameters();
	CVAStruct ClientGetHomogeneousMediumParameters( const CVAStruct& oArgs );
	void ServerGetHomogeneousMediumParameters();


private:
	VistaConnectionIP*			m_pCommandChannel;
	VistaConnectionIP*			m_pHeadChannel;
	VistaConnectionIP*			m_pLastAnswerConnection;

	CVANetClientImpl*			m_pClient;
	CVANetServerImpl*			m_pServer;
	IVAInterface*					m_pRealCore;

	bool						m_bBufferSynchronisedModification;
	bool						m_bBufferingActive;

	CVANetMessage*				m_pMessage;
	CVANetMessage*				m_pBufferedMessage;
	int							m_nCurrentMessageMode;

	CVANetMessage*				m_pEventMessage;
	VistaByteBufferDeSerializer	m_oIncomingEvent;
	std::vector<VANetCompat::byte>			m_vecEventBuffer;

	//VistaByteBufferSerializer	m_oHeadMessage;

	int							m_iExceptionHandlingMode;
	int							m_iReceiveTimeout;
};

#endif // IW_VANET_NETWORK_PROTOCOL
