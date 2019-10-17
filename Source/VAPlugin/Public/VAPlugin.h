// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "Components/SphereComponent.h"

#define VANET_STATIC
#define VABASE_STATIC
#define VA_STATIC

#include "VADefines.h"

#pragma warning(disable:5038) //disable initilization order warning
#include "VA.h"
#include "VANet.h"
#pragma warning(default:5038)
#include "VAUtils.h"
#include "VAReflectionWall.h"


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

	// initialize walls // 
	static void initializeWalls(TArray<AVAReflectionWall*> walls);
    
    // connect to Server (called by initializeServer) //
	static bool connectServer	(FString host = "localhost", int port = 12340);

	// reset Server //
	static bool resetServer();
    
    // check if VA Server is connected //
    static bool isConnected();

	// initialize Sound Source Directivities //
	static bool initializeSoundSourceDirectivities();

	// reset All //
	static bool resetAll();
    
	// static bool ini();


	// static void playTestSound(bool loop = true);

	// initialize sound Sources with their Reflections
	static int initializeSoundWithReflections(FString soundNameF, FVector soundPos = FVector(0, 0, 0), FRotator soundRot = FRotator(0, 0, 0), 
		float gain = 0.0f, bool loop = false, float soundOffset = 0.0f, int action = IVAInterface::VA_PLAYBACK_ACTION_PAUSE, UVASourceComponent* sourceComp = nullptr);

    // initialize sound Source at VA Server //
	static int initializeSound(FString soundNameF, FVector soundPos = FVector(0, 0, 0), FRotator soundRot = FRotator(0, 0, 0),
		float gain = 0.0f, bool loop = false, float soundOffset = 0.0f, int action = IVAInterface::VA_PLAYBACK_ACTION_PAUSE, UVASourceComponent* sourceComp = nullptr);
    
    // enque Sound Component to prevent initialize bevor the connection to VA Server is established //
	static bool enqueueSound(UVASourceComponent* soundComponent);
    
    // process all enqued Sound Sources //
	static bool processSoundQueue();
    
    // change Sound action (play, pause, stop, ..) //
	static bool setSoundAction(int soundID, int soundAction);

	// change Sound action (play, pause, stop, ..) of source and its reflections // 
	static bool setSoundActionWithReflections(int soundID, int soundAction);

	// change Sound time //
	static bool setSoundTime(int soundID, double time);

	// change Sound action time of source and its reflections // 
	static bool setSoundTimeWithReflections(int soundID, double time);

	// static void updateReceiverPos(VAVec3* pos, VAQuat* quat);
	// static void updateReceiverPosRot(FVector& pos, FQuat& quat);
	// static void updateReceiverPosRot(FVector& pos, FRotator& rot);

	// update Sound Source Position and Orientation //
	static bool updateSourcePos(int iSourceID, FVector pos, FQuat quat, UVASourceComponent* sourceComp = nullptr);
	static bool updateSourcePos(int iSourceID, FVector pos, FRotator rot, UVASourceComponent* sourceComp = nullptr);


	// update Sound Source Position and Orientation With Reflections //
	static bool updateSourcePosWithReflections(int iSourceID, FVector pos, FQuat quat, UVASourceComponent* sourceComp = nullptr);
	static bool updateSourcePosWithReflections(int iSourceID, FVector pos, FRotator rot, UVASourceComponent* sourceComp = nullptr);

	// compute Transform of reflected sound Source // 
	static FTransform computeReflectedTransform(AVAReflectionWall* wall, FTransform trans);
	static FTransform computeReflectedTransform(AVAReflectionWall* wall, FVector vec, FRotator rot);

	static FVector computeReflectedPos(AVAReflectionWall* wall, FVector pos);
	static FRotator computeReflectedRot(AVAReflectionWall* wall, FRotator rot);

    // update Receiver Position and Oritentation //
	static bool updateReceiverPos(FVector pos, FQuat quat);
	static bool updateReceiverPos(FVector pos, FRotator rot);
    
    // update Recievers real World Position and Oritentation // 
    static bool updateReceiverRealWorldPos(FVector pos, FQuat quat);
    static bool updateReceiverRealWorldPos(FVector pos, FRotator rot);

    // set Receiver Directivity (HRIR) //
	static bool setReceiverDirectivity(std::string directivity);

	// set Sound Source Directivity with dirName // 
	static bool setSourceDirectivity(int soundID, FString directivity);

	// set Sound Source Directivity with dirID // 
	static bool setSourceDirectivity(int soundID, int dirID);

	// change sound directivity of source and its reflections with dirName // 
	static bool setSourceDirectivityWithReflections(int soundID, FString directivity);

	// change sound directivity of source and its reflections with dirID // 
	static bool setSourceDirectivityWithReflections(int soundID, int dirID);

	// set Sound Source Directivity with Phoneme // 
	static bool setSourceDirectivity_Phoneme(int soundID, FString phoneme);

	// change sound directivity of source and its reflections with Phoneme // 
	static bool setSourceDirectivityWithReflections_Phoneme(int soundID, FString phoneme);

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

    // check if is Master and VA Server should be used // 
	static bool isMasterAndUsed();

	// process / output CVAException //
	static void processExeption(FString location, CVAException e);
	static void processExeption(FString location, FString exception);

	// read Dir file //
	static bool readDirFile(FString dirName);

	// returns if is in Debug Mode //
	static bool isInDebugMode();


	// TODO: //
	// change Dir file //



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

	// Mapping of Sound Component IDs to their Reflection IDs - Make sure to always handle the "normal" instance, too // 
	static TMap<int, TArray<int>> soundComponentsReflectionIDs;

	// Mapping of Sound Reflection to their walls //
	static TMap<int, AVAReflectionWall*> matchingReflectionWalls;

	// Mapping of all Sound Source Directivities to their IDs // 
	static TMap<FString, int> dirMap;

	// Default Sound Source Directivity ID // 
	static int defaultDirID;

	// List of all Reflection Walls in Szene //
	static TArray<AVAReflectionWall*> reflectionWalls;

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

	// bool if VA is used // 
	static bool useVA;

	// bool if is in Debug Mode //
	static bool debugMode;

	// List of all registered input Phonemes //
	static TArray<FString> listOfPhonemes;

	// List of all registered input Pairs //
	static TArray<FString> listOfPairs;

	// List of all actual dirs//
	static TArray<FString> listOfDirectivities;

	// Mapping of all inputs to their phenomes //
	static TMap<FString, FString> dirMapping;

	// Mapping of all phenomes to their int value @VA Server //
	static TMap<FString, int> dirMappingToInt;


    
	// bool runServerTest();



};
