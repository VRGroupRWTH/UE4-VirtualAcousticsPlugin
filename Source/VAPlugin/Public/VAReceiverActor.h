// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VADefines.h"

#include "VADirectivityManager.h"
#include "VAHRIRManager.h"

#include "GameFramework/Actor.h"
#include "Cluster/IDisplayClusterClusterManager.h"		// Events

#include "VAReceiverActor.generated.h"


class AVAReflectionWall;

UENUM()
enum EAddress
{
	Automatic,
	Localhost,
	Cave,
	Manual
};


UCLASS()
class VAPLUGIN_API AVAReceiverActor : public AActor
{
	GENERATED_BODY()

protected:


	// How many units in UE equal 1m in World
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Scale", Category = "General Settigns"))
	float WorldScale = 100.0f;

	// How many units in UE equal 1m in World
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Updates per second", Category = "General Settigns"))
	int UpdateRate = 30;

	// Check if should ask for debug Mode?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Ask for Debug mode?", Category = "General Settigns"))
	bool bAskForDebugMode = true;

	// Choose how to connect to the Server (automatic: build with windows connect with 127.0.0.1:12340, build with linux connect to cave)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Usecase", Category = "Connection"))
	TEnumAsByte<EAddress> AddressSetting = EAddress::Automatic;

	// IP Address for manual input
	UPROPERTY(EditAnywhere, meta = (DisplayName = "IP Adress", Category = "Connection")) // CanEditChange used
	FString ServerIPAddress = "10.0.1.240";

	// Port for manual input
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Port [0, 65535]", Category = "Connection", // CanEditChange used
		ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
	uint16 ServerPort = 12340;

	// Read in File? 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Read an initial mapping file?", Category = "Directivity Manager"))
	bool bReadInitialMappingFile = true;
	
	// File name of the Directivity mapping file
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Name of ini file for directivities", Category = "Directivity Manager"))
	FString DirMappingFileName = "Study/VADir_default.ini";

	// Port for remote VAServer starting
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Remote VAServer Start Port [0, 65535]", Category = "Connection",
		// CanEditChange used
		ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
	uint16 RemoteVAStarterPort = 41578;

	// Which version should be started automatically
	UPROPERTY(EditAnywhere, meta = (DisplayName =
		"Which VAServer version should be started, configurable in the Config of the VAServer Launcher", Category =
		"General Settigns"))
	FString WhichVAServerVersionToStart = TEXT("2018.a");



public:

	// Sets default values for this actor's properties
	AVAReceiverActor();
	void Tick(float DeltaTime) override;

	// Interface for HRIR Settings
	UFUNCTION(BlueprintCallable)
	void SetUpdateRate(int Rate);

	// Interface for Dir Mapping
	UFUNCTION(BlueprintCallable)
	bool ReadDirMappingFile(FString FileName);

	// Interface for HRIR Settings
	UFUNCTION(BlueprintCallable)
	bool SetHRIRByFileName(FString FileName);


	// Directivity Handling
	FVADirectivity* GetDirectivityByMapping(FString Phoneme) const;
	FVADirectivity* GetDirectivityByFileName(FString FileName);


	// Getter Functions
	bool IsInitialized() const;
	float GetScale() const;
	FString GetIPAddress() const;
	int GetPort() const;
	int GetUpdateRate() const;
	TArray<AVAReflectionWall*> GetReflectionWalls();

	static AVAReceiverActor* GetCurrentReceiverActor();


	// Cluster Stuff
	void RunOnAllNodes(FString Command);


protected:
	
	// Initialization
	void BeginPlay() override;
	void BeginDestroy() override;

	// Position updates
	bool UpdateVirtualWorldPose();
	bool UpdateRealWorldPose();


	// Cluster Stuff
	void HandleClusterEvent(const FDisplayClusterClusterEvent& Event);
	void HandleClusterCommand(FString Command);


	void InitializeWalls();

	
#if WITH_EDITOR
	bool CanEditChange(const UProperty* InProperty) const override;
#endif

	// Current Receiver Actor
	static AVAReceiverActor* CurrentReceiverActor;

	
	// Receiver Specific Data
	int ReceiverID;
	FVADirectivityManager DirManager;
	FVAHRIRManager HRIRManager;

	FVAHRIR* CurrentHRIR;

	TArray<AVAReflectionWall*> ReflectionWalls;

	
	// State of Actor
	bool bInitialized = false;
	bool bWallsInitialized = false;


	// Time stuff
	float TimeSinceUpdate;
	float TotalTime;

	// Event Listener Delegate
	FOnClusterEventListener ClusterEventListenerDelegate;

};
