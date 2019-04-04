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

#ifndef IW_VANET_MESSAGE
#define IW_VANET_MESSAGE

#include <VA.h>

#include "VANetVistaCompatibility.h"

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaSerializingToolset.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>

#include <string>

class VistaConnectionIP;

class CVAEvent;
class CVAException;
class CVAVersionInfo;
class CVADirectivityInfo;
class CVASignalSourceInfo;
class CVASceneInfo;

//! Network message helper
/** Messages consist of a message part and an answer part, each read or written
  * separately. Messages have a three-int-header (SIZE, MSGTYPE; EXCEPTIONMODE), while
  * answers have a two-int header (SIZE; ANSWERTYPE)
  */
class CVANetMessage
{
public:
	explicit CVANetMessage( const VistaSerializingToolset::ByteOrderSwapBehavior bSwapBuffers );

	void ResetMessage();

	void SetConnection( VistaConnectionIP* pConn );

	void WriteMessage();
	void ReadMessage();
	void WriteAnswer();
	void ReadAnswer();

	int GetExceptionMode() const;
	int GetMessageType() const;
	int GetAnswerType() const;

	void SetExceptionMode( const int nMode );
	void SetMessageType( const int nType );
	void SetAnswerType( const int nType );

	int GetIncomingMessageSize() const;
	int  GetOutgoingMessageSize() const;
	bool GetOutgoingMessageHasData() const;


	// serializing functions

	void WriteString( const std::string& sValue );
	void WriteInt( const int iValue );
	void WriteUInt64( const uint64_t iValue );
	void WriteBool( const bool bValue );
	void WriteFloat( const float fValue );
	void WriteDouble( const double fValue );
	void WriteVec3( const VAVec3& oVec );
	void WriteQuat( const VAQuat& qOrient );
	void WriteException( const CVAException& oException );
	void WriteEvent( const CVAEvent& oEvent );
	void WriteVersionInfo( const CVAVersionInfo& oInfo );
	void WriteDirectivityInfo( const CVADirectivityInfo& oInfo );
	void WriteSignalSourceInfo( const CVASignalSourceInfo& oInfo );
	void WriteSceneInfo( const CVASceneInfo& oInfo );
	void WriteModuleInfo( const CVAModuleInfo& oInfo );
	void WriteBlob( const void* pBuf, const int nBytes );
	void WriteVAStruct( const CVAStruct& oStruct );
	void WriteAudioRenderingModuleInfo( const CVAAudioRendererInfo& );
	void WriteAudioReproductionModuleInfo( const CVAAudioReproductionInfo& );
	void WriteSoundReceiverInfo( const CVASoundReceiverInfo& oInfo );
	void WriteSoundSourceInfo( const CVASoundSourceInfo& oInfo );
	void WriteSoundPortalInfo( const CVASoundPortalInfo& oInfo );
	void WriteGeometryMesh( const CVAGeometryMesh& oMesh );
	void WriteGeometryMeshFace( const CVAGeometryMesh::CVAFace & oFace );
	void WriteGeometryMeshVertex( const CVAGeometryMesh::CVAVertex & oVertex );

	std::string ReadString();
	int ReadInt();
	uint64_t ReadUInt64();
	bool ReadBool();
	float ReadFloat();
	double ReadDouble();
	VAVec3 ReadVec3();
	VAQuat ReadQuat();
	CVAException ReadException();
	CVAEvent ReadEvent();
	CVAVersionInfo ReadVersionInfo();
	CVADirectivityInfo ReadDirectivityInfo();
	CVASignalSourceInfo ReadSignalSourceInfo();
	CVASceneInfo ReadSceneInfo();
	void ReadModuleInfo( CVAModuleInfo& oInfo );
	void ReadBlob( void* pBuf, const int nBytes );
	void ReadVAStruct( CVAStruct& oStruct );
	CVAAudioRendererInfo ReadAudioRenderingModuleInfo();
	CVAAudioReproductionInfo ReadAudioReproductionModuleInfo();
	CVASoundReceiverInfo ReadSoundReceiverInfo();
	CVASoundSourceInfo ReadSoundSourceInfo();
	CVASoundPortalInfo ReadSoundPortalInfo();
	CVAGeometryMesh ReadGeometryMesh();
	CVAGeometryMesh::CVAFace ReadGeometryMeshFace();
	CVAGeometryMesh::CVAVertex ReadGeometryMeshVertex();

	VistaConnectionIP* GetConnection() const;
	void ClearConnection();
	CVAAcousticMaterial ReadAcousticMaterial();

private:
	int m_nMessageType;
	int m_nMessageId;
	int m_nAnswerType;
	int m_nExceptionMode;
	VistaByteBufferSerializer	m_oOutgoing;
	VistaByteBufferDeSerializer	m_oIncoming;
	std::vector<VANetCompat::byte>			m_vecIncomingBuffer;

	VistaConnectionIP* m_pConnection;
public:
	void WriteAcousticMaterial( const CVAAcousticMaterial& oMaterial );
};

#endif// IW_VANET_MESSAGE
