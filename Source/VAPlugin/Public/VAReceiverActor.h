// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "VADirectivityManager.h"
#include "VAHRIRManager.h"
#include "GameFramework/Actor.h"
#include "Cluster/IDisplayClusterClusterManager.h"		// For Events
#include "Cluster/DisplayClusterClusterEvent.h"			// For Events
#include "CoreMinimal.h"								// For Events

#include "VAReceiverActor.generated.h"

// Foreward declarations
class AVAReflectionWall;


UENUM()
enum EAdress
{
	automatic,
	localhost,
	Cave,
	manual
};


UCLASS()
class VAPLUGIN_API AVAReceiverActor : public AActor
{
	GENERATED_BODY()

protected:


	// How many units in UE equal 1m in World
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Scale", Category = "General Settigns"))
	float worldScale = 100.0f;

	// Check if should ask for debug Mode?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Ask for Debug mode?", Category = "General Settigns"))
	bool askForDebugMode = true;

	// Choose how to connect to the Server (automatic: build with windows connect with 127.0.0.1:12340, build with linux connect to cave)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Usecase", Category = "Connection"))
	TEnumAsByte<EAdress> adressSetting = automatic;

	// IP Adress for manual input
	UPROPERTY(EditAnywhere, meta = (DisplayName = "IP Adress", Category = "Connection")) // CanEditChange used
	FString serverIPAdress = "10.0.1.240";

	// Port for manual input
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Port [0, 65535]", Category = "Connection", // CanEditChange used
		ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
	uint16 serverPort = 12340;

	// File name of the Directivity mapping file
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Name of ini file for directivities", Category = "Directivity Manager"
	))
	FString dirMappingFileName = "Study/VADir_default.ini";

	// Port for remote VAServer starting
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Remote VAServer Start Port [0, 65535]", Category = "Connection",
		// CanEditChange used
		ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
	uint16 remoteVAStarterPort = 41578;

	// Check if the system should try to start the VAServer automatically
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Try to start VAServer automatically?", Category = "General Settigns")
	)
	bool askForAutomaticRemoteVAStart = true;

	// Which version should be started automatically
	UPROPERTY(EditAnywhere, meta = (DisplayName =
		"Which VAServer version should be started, configurable in the Config of the VAServer Launcher", Category =
		"General Settigns"))
	FString whichVAServerVersionToStart = TEXT("2018.a");


public:

	// Sets default values for this actor's properties
	AVAReceiverActor();

	UFUNCTION(BlueprintCallable)
	void readDirMappingFile(FString fileName);

	// Sets Receiver HRIR by File Name
	UFUNCTION(BlueprintCallable)
	void setHRIRByFileName(FString fileName);

	// return Scale
	float getScale();

	// return IP Adress as FString TODO wie sonst?
	FString getIPAdress();

	int getPort();

	VADirectivity* getDirectivityByMapping(FString phoneme);
	VADirectivity* getDirectivityByFileName(FString fileName);

	TArray<AVAReflectionWall*> getReflectionWalls();

	void runOnAllNodes(FString command);

	static AVAReceiverActor* getCurrentReceiverActor();

	bool isInitialized();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void BeginDestroy() override;

	// Updates the Virtual World Position of the Player // 
	bool updateVirtualWorldPosition();

	// Updates the Real World Position of the Player //
	bool updateRealWorldPosition();

	// Link to the PlayerController
	APlayerController* controller;

	// Link to the Player Instance
	AActor* player;

	// var used to measure time since last update
	float timeSinceUpdate;

	// tmp Var to do some experiments
	float totalTime;

	// Tmp Var
	FVector tmpPosF;
	FRotator tmpRotF;
	FQuat tmpQuatF;


	int receiverID;

	// STUFF AFTER CHANGE

	VADirectivityManager dirManager;
	VAHRIRManager hrirManager;

	TArray<AVAReflectionWall*> reflectionWalls;

	bool initialized = false;

	bool wallsInitialized = false;

	void initializeWalls();


	// Event Listener Delegate
	FOnClusterEventListener ClusterEventListenerDelegate;

	// Called on cluster Event emission
	void HandleClusterEvent(const FDisplayClusterClusterEvent& Event);

	// "Compiler"
	void handleClusterCommand(FString command);


#if WITH_EDITOR
	bool CanEditChange(const UProperty* InProperty) const override;
#endif

	static AVAReceiverActor* currentReceiverActor;

public:
	// Called every frame
	void Tick(float DeltaTime) override;
};
