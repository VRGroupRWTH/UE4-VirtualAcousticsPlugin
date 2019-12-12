// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


#include "VAUtils.h"

#define VANET_STATIC
#define VABASE_STATIC
#define VA_STATIC



//forward declarations:
class AVAReceiverActor;
class AVAReflectionWall;
class UVASourceComponent;
class CVAException;
class IVANetClient;
class IVAInterface;
class VAQuat;
class VAVec3;

class FVAPluginModule : public IModuleInterface
{
public:

	// REMAKE!!!
    // check if is Master and VA Server should be used // 
	static bool isMasterAndUsed();

	// process / output CVAException //
	static void processExeption(FString location, CVAException e);
	static void processExeption(FString location, FString exception);


	// returns if is in Debug Mode //
	static bool isInDebugMode();





	// ******* Initialization Functions ******* //

    // Function called when Starting up the module //
	virtual void StartupModule() override;
    
    // Function called when Shutting down the module //
	virtual void ShutdownModule() override;

	// Askes whether to use the VA Server and / or the debug mode
	static void askForSettings(FString host = "unknown", int port = 0, bool askForDebugMode = true);

	// Check if all Library Handles are well initialized //
	static bool checkLibraryHandles();





	// ******* General Server Functions ******* //
    
    // connect to Server (called by initializeServer) //
	static bool connectServer	(FString host = "localhost", int port = 12340);

	// reset Server //
	static bool resetServer();
    
    // check if VA Server is connected //
    static bool isConnected();

	// Disconnect from VA Server 
	static bool disconnectServer();

   



	// ******* Sound Buffer ******* //

	static std::string createNewBuffer(FString soundFileName, bool loop = false, float soundOffset = 0.0f);
	static bool	setSoundBufferAction(std::string sBufferID, EPlayAction action);
	static int  getSoundBufferAction(std::string sBufferID);
	static bool setSoundBufferTime(std::string sBufferID, float time);
	

	// ******* Sound Sources ******* //

	static int  createNewSoundSource(std::string bufferID, std::string name, FVector pos = FVector(0,0,0), FRotator rot = FRotator(0,0,0), float gainFactor = 1.0f);
	static bool setSoundSourcePos(int soundSourceID, FVector pos);
	static bool setSoundSourceRot(int soundSourceID, FRotator rot);
	static bool setNewBufferForSoundSource(int soundSourceID, std::string bufferID);

	// ******* Directivities ******* //

	static int  createNewDirectivity(FString fileName);
	static bool setSoundSourceDirectivity(int soundSourceID, int dirID);


	// ******* Sound Receiver ******* //

	static int  createNewSoundReceiver(AVAReceiverActor* actor);
	static bool setSoundReceiverDirectivity(int soundReceiverID, int dirID);
	static bool setSoundReceiverPosition(int soundReceiverID, FVector pos);
	static bool setSoundReceiverRotation(int soundReceiverID, FRotator rot);


	// ******* Real World ******* //

	static bool setSoundReceiverRealWorldPose(int soundReceiverID, FVector pos, FRotator rot);


	// ******* General Setter Functions ******* //

	static void setReceiverActor(AVAReceiverActor* actor);
	static void setScale(float scale_);
	static void setUseVA(bool useVA_);
	static void setDebugMode(bool debugMode_);


	// ******* Getter Functions ******* //

	static bool getIsInitialized();
	static bool getUseVA();
	static bool getDebugMode();
	static bool getIsMaster();
	static AVAReceiverActor* getReceiverActor();
	static TArray<AVAReflectionWall*> getReflectionWalls();
	

protected:


	// Library Handles for dll loading of VA Classes
	static void*    LibraryHandleBase;
	static void*	LibraryHandleNet;
	static void*    LibraryHandleVistaBase;
	static void*	LibraryHandleVistaAspects;
	static void*	LibraryHandleVistaInterProcComm;


	// Library Handles for dll loading of VA Classes
	static bool initialized;						// To check if its already initialized
	static bool useVA;								// bool if VA is used 
	static bool debugMode;							// bool if is in Debug Mode
	static bool isMaster;							// bool if its the master node 
	

	// Interface Classes to Server 
	static IVANetClient* pVANet;					// VA Net Client
    static IVAInterface* pVA;						// VA Server Interface
    

    // Link to the current receiver actor 
	static AVAReceiverActor* receiverActor;


	// Scale of the UE4 world (how many units is 1m in "real life")
	static float scale;


	// tmp Var for easier usage
	static VAQuat* tmpQuat;
	static VAVec3* tmpVec;

};
