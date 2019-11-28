// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAPlugin.h"

#include "VADirectivityManager.h"

#include "Cluster/IDisplayClusterClusterManager.h"		// For Events
#include "Cluster/DisplayClusterClusterEvent.h"			// For Events

#include "CoreMinimal.h"								// For Events
#include "GameFramework/Actor.h"
#include "VAReceiverActor.generated.h"

// struct used to input Directivity via Variable in UE4 Editor
UENUM()
enum EDir { 
	DefaultHRIR,
	manualFile, 
	phoneme
}; 

UENUM()
enum EAdress {
	automatic,
	localhost, 
	Cave,
	manual
};



UCLASS()
class VAPLUGIN_API AVAReceiverActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVAReceiverActor();

	// Factor for global output Gain // TODO:
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor [0,1] (not working yet)",	Category = "General Settigns",
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float vGainFactor = 1.0f;
	
	// How many units in UE equal 1m in World
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Scale",									Category = "General Settigns"))
		float vScale = 100.0f;

	// Check if should ask for debug Mode?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Ask for Debug mode?",					Category = "General Settigns"))
		bool vAskForDebugMode = true;

	// Choose Where To use the Plugin (Important for IP Adress and Port)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Usecase",								Category = "Connection"))
		TEnumAsByte<EAdress> vAdressType = EAdress::automatic;

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "IP Adress",								Category = "Connection"))		// CanEditChange used
		FString vAdress = "10.0.1.240";

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Port [0, 65535]",						Category = "Connection",		// CanEditChange used
		ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
		uint16 vPort = 12340;

	// Choose Directivity File for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity",							Category = "Directivity"))
		TEnumAsByte<EDir> vDirectivity = EDir::DefaultHRIR;

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name",				Category = "Directivity"))
		FString vDirectivityByFileName = "$(DefaultHRIR)";

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by phoneme",					Category = "Directivity"))
		FString vDirectivityByPhoneme = "";

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Name of ini file for directivities",		Category = "Directivity Manager"))
		FString dirName = "VADir_default.ini";



	// // Factor for global output Gain
	// UPROPERTY(EditAnywhere, meta = (DisplayName = "Ask for Debug mode?"))
	// 	static bool vDebugMode;

	// return directivity x in form: $(x)
	std::string getDirectivity();
	
	// return Offset
	// float getGainFactor();

	// return Scale
	float getScale();

	// return IP Adress as FString TODO wie sonst?
	FString getIPAdress();

	int getPort();

	static VADirectivity* getDirectvityByPhoneme(FString phoneme);

	TArray<AVAReflectionWall*> getReflectionWalls();

	void runOnAllNodes(FString command);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

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
	FVector		tmpPosF;
	FRotator	tmpRotF;
	FQuat		tmpQuatF;
	VAVec3*		tmpPos;
	VAQuat*		tmpQuat;

	bool isMaster;

	int receiverID;

	// STUFF AFTER CHANGE

	static VADirectivityManager dirManager;

	TArray<AVAReflectionWall*> reflectionWalls;

	bool wallsInitialized;

	void initializeWalls();


	// Event Listener Delegate
	FOnClusterEventListener ClusterEventListenerDelegate;

	// Called on cluster Event emission
	void HandleClusterEvent(const FDisplayClusterClusterEvent& Event);

	// "Compiler"
	void handleClusterCommand(FString command);


#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const;
#endif

	bool trash;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
