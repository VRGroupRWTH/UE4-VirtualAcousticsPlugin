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
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static bool connectServer(FString host = "localhost");
	static bool initializeReceiver(AVAReceiverActor * actor);
	static bool initializeServer();
	static bool ini();


	static void playTestSound(bool loop = true);

	static int initializeSound(FString soundNameF, FVector soundPos = FVector(0, 0, 0), FRotator soundRot = FRotator(0, 0, 0), float gain = 0.0f, bool loop = false, float soundOffset = 0.0f, int action = IVAInterface::VA_PLAYBACK_ACTION_PAUSE);
	static bool enqueueSound(UVASourceComponent* soundComponent);
	static bool processSoundQueue();
	static bool setSoundAction(int soundID, int soundAction);

	static void updateReceiverPos(VAVec3* pos, VAQuat* quat);
	static void updateReceiverPosRot(FVector& pos, FQuat& quat);
	static void updateReceiverPosRot(FVector& pos, FRotator& rot);

	static bool updateSourcePos(int iSourceID, FVector pos, FQuat quat);
	static bool updateSourcePos(int iSourceID, FVector pos, FRotator rot);

	static bool updateReceiverPos(FVector pos, FQuat quat);
	static bool updateReceiverPos(FVector pos, FRotator rot);

	static bool setReceiverDirectivity(std::string directivity);




	static bool setViewMode();
	static VAUtils::viewEnum getViewMode();
	static bool isViewModeUnclear();
	static bool isViewModeThirdPerson();
	static bool isViewModeHMD();
	static bool isViewModeCave();

	static bool isConnected();

	static VAQuat* tmpQuat;
	static VAVec3* tmpVec;

protected:
	/** Handle to the test dll we will load */
	//void*	ExampleLibraryHandle;

	static IVANetClient* pVANet;
	static IVAInterface* pVA;

	static AVAReceiverActor* receiverActor;

	static TArray<UVASourceComponent*> uninitializedSoundComponents;
	static TArray<UVASourceComponent*> soundComponents;
	static TMap<int, std::string> soundComponentsIDs;

	static VAUtils::viewEnum viewMode;
	static float scale;
	
	void* ExampleLibraryHandle;

	static void*	LibraryHandleNet;
	static void*	LibraryHandleBase;
	static void*	LibraryHandleVistaAspects;
	static void*	LibraryHandleVistaBase;
	static void*	LibraryHandleVistaInterProcComm;

	static int iSoundReceiverID;
	static int iSoundSourceID;
	static int iHRIR;

	static std::string directivity;

	//static void openMessageBox(char* text, bool error = false);
	//static void openMessageBoxV(char* text, bool error = false);
	//static char* addExclamationMarkInChar(char* text);

	static bool checkLibraryHandles(bool einzeln);
	bool runServerTest();



};
