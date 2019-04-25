// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

#include "VA.h"
#include "VANet.h"
#include "VAUtils.h"

class AVAReceiverActor;
class UVASourceComponent;

class FVAPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
    // Function called when Starting up the module //
	virtual void StartupModule() override;
    
    // Function called when Shutting down the module //
	virtual void ShutdownModule() override;

    // initialize & Create Receiver at VA Server //
	static bool initializeReceiver(AVAReceiverActor * actor);
    
    // initialize Server (and connect) //
	static bool initializeServer(FString host = "localhost", int port = 12340);
    
    // connect to Server (called by initializeServer) //
	static bool connectServer	(FString host = "localhost", int port = 12340);
    
    // check if VA Server is connected //
    static bool isConnected();
    
	// static bool ini();


	// static void playTestSound(bool loop = true);

    // initialize sound Source at VA Server //
	static int initializeSound(FString soundNameF, FVector soundPos = FVector(0, 0, 0), FRotator soundRot = FRotator(0, 0, 0), float gain = 0.0f, bool loop = false, float soundOffset = 0.0f, int action = IVAInterface::VA_PLAYBACK_ACTION_PAUSE);
    
    // enque Sound Component to prevent initialize bevor the connection to VA Server is established //
	static bool enqueueSound(UVASourceComponent* soundComponent);
    
    // process all enqued Sound Sources //
	static bool processSoundQueue();
    
    // change Sound action (play, pause, stop, ..) //
	static bool setSoundAction(int soundID, int soundAction);

	// static void updateReceiverPos(VAVec3* pos, VAQuat* quat);
	// static void updateReceiverPosRot(FVector& pos, FQuat& quat);
	// static void updateReceiverPosRot(FVector& pos, FRotator& rot);

    // update Sound Source Position and Orientation //
	static bool updateSourcePos(int iSourceID, FVector pos, FQuat quat);
	static bool updateSourcePos(int iSourceID, FVector pos, FRotator rot);

    // update Receiver Position and Oritentation //
	static bool updateReceiverPos(FVector pos, FQuat quat);
	static bool updateReceiverPos(FVector pos, FRotator rot);

    // set Receiver Directivity (HRIR) //
	static bool setReceiverDirectivity(std::string directivity);


    // set View Mode //
	static bool setViewMode();
    
    // get View Mode //
	static VAUtils::viewEnum getViewMode();
    
    // check if View Mode is unclear //
	static bool isViewModeUnclear();
    
    // check if View Mode is Third Person //
	static bool isViewModeThirdPerson();
	
    // check if View Mode is HMD //
    static bool isViewModeHMD();
    
    // check if View Mode is Cave //
	static bool isViewModeCave();

    


protected:

    // VA Net Client //
	static IVANetClient* pVANet;
	
    // VA Server Interface //
    static IVAInterface* pVA;

    // Link to Receiver Actor //
	static AVAReceiverActor* receiverActor;

    // List of unitialized Sound Components at beginning //
	static TArray<UVASourceComponent*> uninitializedSoundComponents;
    
    // List of all Sound Components TODO: use and ini it //
	static TArray<UVASourceComponent*> soundComponents;
    
    // Mapping of Sound Component IDs to their names //
	static TMap<int, std::string> soundComponentsIDs;

    // View Mode (unclear, Third Person, HMD, Cave) //
	static VAUtils::viewEnum viewMode;
    
    // Number of units in UE4 that equals 1m //
	static float scale;
	
    // TMP VAQuat //
    static VAQuat* tmpQuat;
    
    // TMP VAVec3 //
    static VAVec3* tmpVec;
    
    // Library Handle for VABase //
    static void*    LibraryHandleBase;
    
    // Library Handle for VANets //
	static void*	LibraryHandleNet;
   
    // Library Handle for VistaBase //
    static void*    LibraryHandleVistaBase;
    
    // Library Handle for VistaAspect //
	static void*	LibraryHandleVistaAspects;
    
    // Library Handle for VistaInterProcComm //
	static void*	LibraryHandleVistaInterProcComm;

    // ID of Sound Receiver // // TODO: move to Actor //
	static int iSoundReceiverID;
    
	// TODO: check if needed //
	static int iSoundSourceID;
    static int iHRIR;
	static std::string directivity;

	//static void openMessageBox(char* text, bool error = false);
	//static void openMessageBoxV(char* text, bool error = false);
	//static char* addExclamationMarkInChar(char* text);

	// Check if all Library Handles are well initialized //
    static bool checkLibraryHandles(bool einzeln);
    
    
	// bool runServerTest();



};
