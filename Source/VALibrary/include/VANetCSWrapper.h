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

#ifndef INCLUDE_WATCHER_VA_CS_WRAPPER
#define INCLUDE_WATCHER_VA_CS_WRAPPER

#include <VACSDefinitions.h>

#include <VA.h>

#include <string>
#include <vector>

class CUnmanagedVANetClient;

extern "C"
{
	// Network communication

	VACS_API CUnmanagedVANetClient* NativeCreateNetClient();
	VACS_API bool NativeConnectLocalNetClient( CUnmanagedVANetClient* pClient );
	VACS_API bool NativeConnectNetClient( CUnmanagedVANetClient*, const char* pcServerIP, int iPort );
	VACS_API bool NativeGetNetClientConnected( CUnmanagedVANetClient* pClient );
	VACS_API bool NativeDisconnectNetClient( CUnmanagedVANetClient* pClient );
	VACS_API void NativeDisposeNetClient( CUnmanagedVANetClient* pClient );
	VACS_API int NativeGetState( CUnmanagedVANetClient* pClient );
	VACS_API void NativeInitialize( CUnmanagedVANetClient* pClient );
	VACS_API bool NativeAddSearchPath( CUnmanagedVANetClient* pClient, const char* pcSearchPath );
	VACS_API void NativeFinalize( CUnmanagedVANetClient* pClient );
	VACS_API void NativeReset( CUnmanagedVANetClient* pClient );
	VACS_API void NativeCallModule( CUnmanagedVANetClient*, const char* pcModuleID, const CVAStruct* oArgs, CVAStruct* oReturn );

	VACS_API int NativeCreateDirectivityFromFile( CUnmanagedVANetClient*, const char* pcFilePath, const char* pcName );
	VACS_API bool NativeDeleteDirectivity( CUnmanagedVANetClient*, int iDirID );


	// Signal sources
	
	VACS_API int NativeCreateSignalSourceBufferFromFile( CUnmanagedVANetClient*, const char* pcFilePath, const char* pcName, char* pcIdentifier );
	VACS_API void NativeCreateSignalSourceSequencer( CUnmanagedVANetClient*, const char* pcName, char* pcIdentifier );
	VACS_API void NativeCreateSignalSourceNetworkStream( CUnmanagedVANetClient*, const char* pcInterface, int iPort, const char* pcName, char* pcIdentifier );
	VACS_API void NativeCreateSignalSourceTextToSpeech( CUnmanagedVANetClient*, const char* pcName, char* pcIdentifier );
	VACS_API bool NativeDeleteSignalSource( CUnmanagedVANetClient*, const char* pcIdentifier );
	VACS_API void NativeGetSignalSourceInfo( CUnmanagedVANetClient*, const char* pcSignalSourceID, CVASignalSourceInfo& );
	VACS_API void NativeGetSignalSourceInfos( CUnmanagedVANetClient*, std::vector<CVASignalSourceInfo>& vssiDest );
	VACS_API void NativeGetSignalSourceBufferPlaybackState( CUnmanagedVANetClient*, const char* pcSignalSourceID, char* pcPlaybackState );
	VACS_API void NativeSetSignalSourceBufferPlaybackAction( CUnmanagedVANetClient*, const char* pcSignalSourceID, const char* pcPlaybackAction );
	VACS_API void NativeSetSignalSourceBufferPlaybackPosition( CUnmanagedVANetClient*, const char* pcSignalSourceID, double dPlaybackPosition );
	VACS_API bool NativeGetSignalSourceBufferLooping( CUnmanagedVANetClient*, const char* pcSignalSourceID );
	VACS_API void NativeSetSignalSourceBufferLooping( CUnmanagedVANetClient*, const char* pcSignalSourceID, bool bLooping );
	VACS_API void NativeGetSignalSourceParameters( CUnmanagedVANetClient*, const char* pcSignalSourceID, const CVAStruct* oParams, CVAStruct* oResult );
	VACS_API void NativeSetSignalSourceParameters( CUnmanagedVANetClient*, const char* pcSignalSourceID, CVAStruct* oParams );
	
	
	// Update sync
	
	VACS_API bool NativeGetUpdateLocked( CUnmanagedVANetClient* pClient );
	VACS_API void NativeLockUpdate( CUnmanagedVANetClient* pClient );
	VACS_API int NativeUnlockUpdate( CUnmanagedVANetClient* pClient );


	// Sound sources

	VACS_API int NativeCreateSoundSource( CUnmanagedVANetClient*, const char* pcName );
	VACS_API int NativeCreateSoundSourceExplicitRenderer( CUnmanagedVANetClient*, const char* pcRendererID, const char* pcName );
	VACS_API int NativeDeleteSoundSource( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundSourceEnabled( CUnmanagedVANetClient*, int iID, bool bEnabled );
	VACS_API bool NativeGetSoundSourceEnabled( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeGetSoundSourceName( CUnmanagedVANetClient*, int iID, char* pcName );
	VACS_API void NativeSetSoundSourceName( CUnmanagedVANetClient*, int iID, const char* pcName );
	VACS_API void NativeGetSoundSourceSignalSource( CUnmanagedVANetClient*, int iID, char* pcSignalSourceID );
	VACS_API void NativeSetSoundSourceSignalSource( CUnmanagedVANetClient*, int iID, const char* pcSignalSourceID );
	VACS_API int NativeGetSoundSourceAuralizationMode( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundSourceAuralizationMode( CUnmanagedVANetClient*, int iID, const char* pcAuralizationMode );
	VACS_API void NativeGetSoundSourceParameters( CUnmanagedVANetClient*, int iID, CVAStruct* oArgs );
	VACS_API void NativeSetSoundSourceParameters( CUnmanagedVANetClient*, int iID, CVAStruct* oParams, CVAStruct* oResult );
	VACS_API int NativeGetSoundSourceDirectivity( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundSourceDirectivity( CUnmanagedVANetClient*, int iID, int iDirectivityID );
	VACS_API double NativeGetSoundSourceSoundPower( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundSourceSoundPower( CUnmanagedVANetClient*, int iID, double dGain );
	VACS_API bool NativeGetSoundSourceMuted( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundSourceMuted( CUnmanagedVANetClient*, int iID, bool bMuted );
	VACS_API void NativeGetSoundSourcePose( CUnmanagedVANetClient*, int iID, double& px, double& py, double& pz, double& ox, double& oy, double& oz, double& ow );
	VACS_API void NativeSetSoundSourcePose( CUnmanagedVANetClient*, int iID, double px, double py, double pz, double ox, double oy, double oz, double ow );
	VACS_API void NativeGetSoundSourcePosition( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z );
	VACS_API void NativeSetSoundSourcePosition( CUnmanagedVANetClient*, int iID, double x, double y, double z );
	VACS_API void NativeGetSoundSourceOrientation( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z, double& w );
	VACS_API void NativeSetSoundSourceOrientation( CUnmanagedVANetClient*, int iID, double x, double y, double z, double w );
	VACS_API void NativeGetSoundSourceOrientationVU( CUnmanagedVANetClient*, int iID, double& vx, double& vy, double& vz, double& ux, double& uy, double& uz );
	VACS_API void NativeSetSoundSourceOrientationVU( CUnmanagedVANetClient*, int iID, double vx, double vy, double vz, double ux, double uy, double uz );
	
	
	// Sound receiver

	VACS_API int NativeCreateSoundReceiver( CUnmanagedVANetClient*, const char* pcName );
	VACS_API int NativeCreateSoundReceiverExplicitRenderer( CUnmanagedVANetClient*, const char* pcRendererID, const char* pcName );
	VACS_API int NativeDeleteSoundReceiver( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundReceiverEnabled( CUnmanagedVANetClient*, int iID, bool bEnabled );
	VACS_API bool NativeGetSoundReceiverEnabled( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeGetSoundReceiverName( CUnmanagedVANetClient*, int iID, char* pcName );
	VACS_API void NativeSetSoundReceiverName( CUnmanagedVANetClient*, int iID, const char* pcName );
	VACS_API int NativeGetSoundReceiverAuralizationMode( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundReceiverAuralizationMode( CUnmanagedVANetClient*, int iID, const char* pcAuralizationMode );
	VACS_API void NativeGetSoundReceiverParameters( CUnmanagedVANetClient*, int iID, CVAStruct* oArgs );
	VACS_API void NativeSetSoundReceiverParameters( CUnmanagedVANetClient*, int iID, CVAStruct* oParams, CVAStruct* oResult );
	VACS_API int NativeGetSoundReceiverDirectivity( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeSetSoundReceiverDirectivity( CUnmanagedVANetClient*, int iID, int iDirectivityID );

	VACS_API void NativeGetSoundReceiverPose( CUnmanagedVANetClient*, int iID, double& px, double& py, double& pz, double& ox, double& oy, double& oz, double& ow );
	VACS_API void NativeSetSoundReceiverPose( CUnmanagedVANetClient*, int iID, double px, double py, double pz, double ox, double oy, double oz, double ow );
	VACS_API void NativeGetSoundReceiverPosition( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z );
	VACS_API void NativeSetSoundReceiverPosition( CUnmanagedVANetClient*, int iID, double x, double y, double z );
	VACS_API void NativeGetSoundReceiverOrientation( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z, double& w );
	VACS_API void NativeSetSoundReceiverOrientation( CUnmanagedVANetClient*, int iID, double x, double y, double z, double w );
	VACS_API void NativeGetSoundReceiverOrientationVU( CUnmanagedVANetClient*, int iID, double& vx, double& vy, double& vz, double& ux, double& uy, double& uz );
	VACS_API void NativeSetSoundReceiverOrientationVU( CUnmanagedVANetClient*, int iID, double vx, double vy, double vz, double ux, double uy, double uz );
	
	VACS_API void NativeGetSoundReceiverHeadAboveTorsoOrientation( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z, double& w );
	VACS_API void NativeSetSoundReceiverHeadAboveTorsoOrientation( CUnmanagedVANetClient*, int iID, double x, double y, double z, double w );

	VACS_API void NativeGetSoundReceiverRealWorldPositionOrientationVU( CUnmanagedVANetClient*, int iID, double& px, double& py, double& pz, double& vx, double& vy, double& vz, double& ux, double& uy, double& uz );
	VACS_API void NativeSetSoundReceiverRealWorldPositionOrientationVU( CUnmanagedVANetClient*, int iID, double px, double py, double pz, double vx, double vy, double vz, double ux, double uy, double uz );
	VACS_API void NativeGetSoundReceiverRealWorldPose( CUnmanagedVANetClient*, int iID, double& px, double& py, double& pz, double& ox, double& oy, double& oz, double& wo );
	VACS_API void NativeSetSoundReceiverRealWorldPose( CUnmanagedVANetClient*, int iID, double px, double py, double pz, double ox, double oy, double oz, double ow );
	
	VACS_API void NativeGetSoundReceiverRealWorldHeadAboveTorsoOrientation( CUnmanagedVANetClient*, int iID, double& x, double& y, double& z, double& w );
	VACS_API void NativeSetSoundReceiverRealWorldHeadAboveTorsoOrientation( CUnmanagedVANetClient*, int iID, double x, double y, double z, double w );


	// Homogeneous medium

	VACS_API void NativeSetHomogeneousMediumSoundSpeed( CUnmanagedVANetClient*, const double dSoundSpeed );
	VACS_API double NativeGetHomogeneousMediumSoundSpeed( CUnmanagedVANetClient* );
	VACS_API void NativeSetHomogeneousMediumTemperature( CUnmanagedVANetClient*, const double dDegreesCentigrade );
	VACS_API double NativeGetHomogeneousMediumTemperature( CUnmanagedVANetClient* );
	VACS_API void NativeSetHomogeneousMediumStaticPressure( CUnmanagedVANetClient*, const double dPressurePascal );
	VACS_API double NativeGetHomogeneousMediumStaticPressure( CUnmanagedVANetClient* );
	VACS_API void NativeSetHomogeneousMediumRelativeHumidity( CUnmanagedVANetClient*, const double dRelativeHumidityPercent );
	VACS_API double NativeGetHomogeneousMediumRelativeHumidity( CUnmanagedVANetClient* );
	VACS_API void NativeSetHomogeneousMediumShiftSpeed( CUnmanagedVANetClient*, const double x, const double y, const double z );
	VACS_API void NativeGetHomogeneousMediumShiftSpeed( CUnmanagedVANetClient*, double& x, double& y, double& z );
	VACS_API void NativeSetHomogeneousMediumParameters( CUnmanagedVANetClient*, CVAStruct& oParams );
	VACS_API void NativeGetHomogeneousMediumParameters( CUnmanagedVANetClient*, CVAStruct& oArgs, CVAStruct& oResult );


	// Scenes

	VACS_API void NativeCreateSceneFromFile( CUnmanagedVANetClient*, const char* pcFilePath, char* pcSceneID );
	VACS_API void NativeGetSceneName( CUnmanagedVANetClient* pClient, const char* pcID, char* pcName );
	VACS_API void NativeSetSceneName( CUnmanagedVANetClient* pClient, const char* pcID, const char* pcName );
	VACS_API bool NativeGetSceneEnabled( CUnmanagedVANetClient* pClient, const char* pcID );
	VACS_API void NativeSetSceneEnabled( CUnmanagedVANetClient* pClient, const char* pcID, bool bEnabled );


	// Geometry meshes

	VACS_API int NativeCreateGeometryMesh( CUnmanagedVANetClient*, const CVAGeometryMesh* pMesh );
	VACS_API int NativeCreateGeometryMeshFromFile( CUnmanagedVANetClient*, const char* pcFilePath );
	VACS_API void NativeDeleteGeometryMesh( CUnmanagedVANetClient*, int iID );
	VACS_API void NativeGetGeometryMeshName( CUnmanagedVANetClient*, int iID, char* pcName );
	VACS_API void NativeSetGeometryMeshName( CUnmanagedVANetClient*, int iID, const char* pcName );
	VACS_API bool NativeGetGeometryMeshEnabled( CUnmanagedVANetClient* pClient, int iID );
	VACS_API void NativeSetGeometryMeshEnabled( CUnmanagedVANetClient* pClient, int iID, bool bEnabled );
		
	
	// Global control

	VACS_API bool NativeGetInputMuted( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetInputMuted( CUnmanagedVANetClient*, bool bMuted );
	VACS_API double NativeGetInputGain( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetInputGain( CUnmanagedVANetClient*, double dGain );
	VACS_API double NativeGetOutputGain( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetOutputGain( CUnmanagedVANetClient*, double dGain );
	VACS_API bool NativeGetOutputMuted( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetOutputMuted( CUnmanagedVANetClient*, bool bMuted );
	
	VACS_API int NativeGetGlobalAuralizationMode( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetGlobalAuralizationMode( CUnmanagedVANetClient*, const char* pcAuralizationMode );

	VACS_API int NativeGetActiveSoundReceiver( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetActiveSoundReceiver( CUnmanagedVANetClient*, int iSoundReceiverID );
	VACS_API int NativeGetActiveSoundReceiverExplicitRenderer( CUnmanagedVANetClient* pClient, const char* pcRendererID );
	VACS_API void NativeSetActiveSoundReceiverExplicitRenderer( CUnmanagedVANetClient*, int iSoundReceiverID, const char* pcRendererID );
	
	VACS_API double NativeGetCoreClock( CUnmanagedVANetClient* pClient );
	VACS_API void NativeSetCoreClock( CUnmanagedVANetClient*, double dSeconds );
	
	VACS_API void NativeSetRenderingModuleMuted( CUnmanagedVANetClient*, const char* pcModuleID, bool bMuted );
	VACS_API bool NativeGetRenderingModuleMuted( CUnmanagedVANetClient*, const char* pcModuleID );
	VACS_API void NativeSetRenderingModuleGain( CUnmanagedVANetClient*, const char* pcModuleID, double dGain );
	VACS_API double NativeGetRenderingModuleGain( CUnmanagedVANetClient*, const char* pcModuleID );
	
	VACS_API void NativeSetReproductionModuleMuted( CUnmanagedVANetClient*, const char* pcModuleID, bool bMuted );
	VACS_API bool NativeGetReproductionModuleMuted( CUnmanagedVANetClient*, const char* pcModuleID );
	VACS_API void NativeSetReproductionModuleGain( CUnmanagedVANetClient*, const char* pcModuleID, double dGain );
	VACS_API double NativeGetReproductionModuleGain( CUnmanagedVANetClient*, const char* pcModuleID );


	// Special C# functions (not part of VABase interface)

	VACS_API void NativeSetArtificialReverberationTime( CUnmanagedVANetClient*, const char* pcRendererID, double dReverberationTime );
	VACS_API void NativeSetArtificialRoomVolume( CUnmanagedVANetClient*, const char* pcRendererID, double dVolume );
	VACS_API void NativeSetArtificialSurfaceArea( CUnmanagedVANetClient*, const char* pcRendererID, double dArea );

	VACS_API void NativeSetSoundReceiverAnthropometricData( CUnmanagedVANetClient*, int iSoundReceiverID, double dHeadWidth, double dHeadHeight, double dHeadDepth );

	VACS_API void NativeTextToSpeechPrepareTextAndPlaySpeech( CUnmanagedVANetClient*, const char* pcSignalSourceIdentifier, const char* pcText );
	VACS_API void NativeTextToSpeechPrepareText( CUnmanagedVANetClient*, const char* pcSignalSourceIdentifier, const char* pcTextIdentifier, const char* pcText );
	VACS_API void NativeTextToSpeechPlaySpeech( CUnmanagedVANetClient*, const char* pcSignalSourceIdentifier, const char* pcTextIdentifier );

	VACS_API int NativeUpdateGenericPath( CUnmanagedVANetClient * pClient, const char* pcRendererID, int iSourceID, int iReceiverID, int iChannel, double dDelaySeconds, int iNumSamples, double* vdSampleBuffer );
	VACS_API int NativeUpdateGenericPathFromFile( CUnmanagedVANetClient * pClient, const char* pcRendererID, int iSourceID, int iReceiverID, const char* pcFilePath );
}

#endif // INCLUDE_WATCHER_VA_CS_WRAPPER
