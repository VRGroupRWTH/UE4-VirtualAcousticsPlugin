#pragma once

#include "Modules/ModuleManager.h"
#include <string>						// std::string
#include "VAEnums.h"					// EPlayState
#include "VAServerLauncher.h"

#define VANET_STATIC
#define VABASE_STATIC
#define VA_STATIC


//forward declarations:
class AVAReceiverActor;		 
class AVAReflectionWall;
class UVAAbstractSignalSource;

// Interface Classes
class CVAException;			
class IVANetClient;
class IVAInterface;
class VAQuat;
class VAVec3;
class CVAStruct;

class FVAPlugin : public IModuleInterface
{
public:

	// process / output CVAException //
	static void ProcessException(FString Location, CVAException Exception);
	static void ProcessException(FString Location, FString ExceptionString);


	// ******* Initialization Functions ******* //

	// Function called when Starting up the module //
	void StartupModule() override;

	// Function called when Shutting down the module //
	void ShutdownModule() override;

	// Asks whether to use the VA Server and / or the debug mode
	static void AskForSettings(FString Host = "unknown", int Port = 0, bool bAskForDebugMode = true,
	                           bool bAskForUseVA = true);

	// Check if all Library Handles are well initialized //
	static bool CheckLibraryHandles();


	// ******* General Server Functions ******* //

	// connect to Server (called by initializeServer) //
	static bool ConnectServer(FString HostF, int Port);

	// reset Server //
	static bool ResetServer();

	// check if VA Server is connected //
	static bool IsConnected();

	// Disconnect from VA Server 
	static bool DisconnectServer();

	void BeginSession(bool bSomething);
	void EndSession(bool bSomething);

	static void AddVAServerSearchPath(const std::string& SearchPath);


	// ******* Signal Sources ******* //

	static std::string CreateSignalSourceBuffer(const FString& SoundFileName, bool bLoop = false, float SoundOffset = 0.0f);
	static bool SetSignalSourceBufferPlayAction(const std::string& SignalSourceID, EPlayAction::Type Action);
	static int GetSignalSourceBufferPlayAction(const std::string& SignalSourceID);
	static bool SetSignalSourceBufferPlaybackPosition(const std::string& SignalSourceID, float Time);
	static bool SetSignalSourceBufferLooping(const std::string& SignalSourceID, bool bLoop);

	static std::string CreateSignalSourcePrototype(UVAAbstractSignalSource* SignalSource);
	// Deletes a signal source with given ID. Use with great care!
	static bool DeleteSignalSource(const std::string& SignalSourceID);
	static bool SetSignalSourceParameter(const std::string& SignalSourceID, std::string sParamName, float fParamValue);
	static bool SetJetEngineRMP(const std::string& SignalSourceID, float fRPM);


	// ******* Sound Sources ******* //

	static int CreateNewSoundSource(const std::string& Name, FVector Pos = FVector(0, 0, 0),
		FRotator Rot = FRotator(0, 0, 0), float Power = -1.0f, const std::string& SignalSourceID = "");
	// Deletes a sound source with given ID. Use with great care!
	static bool DeleteSoundSource(int SoundSourceID);
	static bool SetSoundSourcePosition(int SoundSourceID, FVector Pos);
	static bool SetSoundSourceRotation(int SoundSourceID, FRotator Rot);
	static bool SetSoundSourceSignalSource(int SoundSourceID, const std::string& SignalSourceID);
	static bool SetSoundSourceMuted(int SoundSourceID, bool bMuted);
	static bool SetSoundSourcePower(int SoundSourceID, float Power);


	// ******* Directivities ******* //

	static int CreateNewDirectivity(FString FileName);
	static bool SetSoundSourceDirectivity(int SoundSourceID, int DirectivityID);
	static bool RemoveSoundSourceDirectivity(int SoundSourceID);


	// ******* HRIR ******* //

	static int CreateNewHRIR(FString FileName);
	static bool SetSoundReceiverHRIR(int SoundReceiverID, int HRIRID);


	// ******* Sound Receiver ******* //

	static int CreateNewSoundReceiver(AVAReceiverActor* Actor);
	static bool SetSoundReceiverPosition(int SoundReceiverID, FVector Pos);
	static bool SetSoundReceiverRotation(int SoundReceiverID, FRotator Rot);


	// ******* Real World ******* //

	static bool SetSoundReceiverRealWorldPose(int SoundReceiverID, FVector Pos, FRotator Rot);

	// ******* Renderer Related ******* //

	static bool RendererEnabled(const std::string& RendererID);
	//Returns whether given renderer is muted (0 or 1). Returns -1 if could VAServer could not be accessed.
	static int GetRendererMuted(const std::string& RendererID);
	static bool SetRendererMute(const std::string& RendererID, const bool bMute);
	static bool SetRendererParameters(const std::string& RendererID, const CVAStruct& RendererParameterStruct);
	

	// ******* Auralization Mode ******* //

	static bool SetGlobalAuralizationMode(int AuraModeBitvector);
	static bool SetRenderingModuleAuralizationMode(const std::string& RendererID, int AuraModeBitvector);


	// ******* General Setter Functions ******* //

	static void SetReceiverActor(AVAReceiverActor* Actor);
	static void SetScale(float ScaleN);
	static void SetUseVA(bool bUseVAN);
	static void SetDebugMode(bool DebugModeN);


	// ******* Getter Functions ******* //

	static bool GetIsInitialized();
	static bool GetWasStarted();
	static bool GetUseVA();
	static bool GetDebugMode();
	static bool ShouldInteractWithServer();
	static AVAReceiverActor* GetReceiverActor();

	
	// ******* VAServer Launcher ******* //
	
	static FVAServerLauncher VAServerLauncher;


protected:


	// Library Handles for dll loading of VA Classes
	static void* LibraryHandleBase;
	static void* LibraryHandleNet;
	static void* LibraryHandleVistaBase;
	static void* LibraryHandleVistaAspects;
	static void* LibraryHandleVistaInterProcComm;


	// States of the plugin
	static bool bPluginInitialized; // To check if its already initialized
	static bool bWasStarted; //to check whether it was at all started (not just Editor open and close again)
	static bool bUseVA; // bool if VA is used 
	static bool bDebugMode; // bool if is in Debug Mode

	// Interface Classes to Server 
	static IVANetClient* VANetClient; // VA Net Client
	static IVAInterface* VAServer; // VA Server Interface


	// Link to the current receiver actor 
	static AVAReceiverActor* ReceiverActor;


	// Scale of the UE4 world (how many units is 1m in "real life")
	static float WorldScale;


	// tmp Var for easier usage
	static TSharedPtr<VAQuat> TmpVAQuatSharedPtr;
	static TSharedPtr<VAVec3> TmpVAVec3SharedPtr;

	static VAQuat* TmpVAQuat;
	static VAVec3* TmpVAVec3;

};
