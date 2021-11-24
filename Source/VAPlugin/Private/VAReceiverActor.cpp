// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"

#include "VAPlugin.h"
#include "VAUtils.h"
#include "VADefines.h"

#include "SoundSource/VAAbstractSourceComponent.h"
#include "ImageSourceModel/VAReflectionWall.h"
#include "Directivity/VADirectivityManager.h"
#include "HRIR/VAHRIRManager.h"


#include "Cluster/DisplayClusterClusterEvent.h"
#include "Engine/World.h"							// World
#include "GameFramework/PlayerController.h"			// Viewport
#include "IDisplayCluster.h"						// For Events
#include "Pawn/VirtualRealityPawn.h"						// VR Pawn
#include "Kismet/GameplayStatics.h"					// Get Actors of class
#include "Utility/VirtualRealityUtilities.h"


// ****************************************************************** // 
// ******* Initialization Functions ********************************* //
// ****************************************************************** //

AVAReceiverActor::AVAReceiverActor()
{
	PrimaryActorTick.bCanEverTick = true;

  DirManager = MakeShared<FVADirectivityManager>();
  HRIRManager = MakeShared<FVAHRIRManager>();

  AuralizationModeController = CreateDefaultSubobject<UVAAuralizationModeController>(TEXT("AuralizationModeController"));
}

void AVAReceiverActor::BeginPlay()
{
	Super::BeginPlay();

	FVAPlugin::SetReceiverActor(this);

	// Cluster Stuff for Events //
	RunOnAllNodesEvent.Attach(this);

	// CurrentReceiverActor = this;
	if(bReconnecToVAServer && UVirtualRealityUtilities::IsMaster() && FVAPlugin::IsConnected())
	{
		//this might be needed if different server version should be started between levels
		FVAPlugin::DisconnectServer();
	}

	//try to start (remote) VAServer automatically
	bool bStartedVAServer = false;
	if (bAutomaticRemoteVAStart)
	{
		FVAPlugin::VAServerLauncher.StartVAServerLauncher(); //if possible
		bStartedVAServer = FVAPlugin::VAServerLauncher.RemoteStartVAServer(GetIPAddress(), RemoteVAStarterPort, WhichVAServerVersionToStart);
		if(bStartedVAServer){
			FVAPlugin::SetUseVA(true);
		}
	}
	
	// Ask if used or not
	FVAPlugin::AskForSettings(GetIPAddress(), GetPort(), bAskForDebugMode, !bStartedVAServer);

	if (UVirtualRealityUtilities::IsMaster())
	{
		if (FVAPlugin::GetUseVA())
		{
			RunOnAllNodes("useVA = true");
		}
		else
		{
			RunOnAllNodes("useVA = false");
			return;
		}
	}

	bWallsInitialized = false;

	TimeSinceUpdate = 0.0f;
	TotalTime = 0.0f;

	if (!FVAPlugin::GetUseVA())
	{
		return;
	}


	if (UVirtualRealityUtilities::IsMaster())
	{
		FVAPlugin::SetScale(WorldScale);

		if (!FVAPlugin::IsConnected())
		{
			FVAPlugin::ConnectServer(GetIPAddress(), GetPort());
		}
		else
		{
			FVAPlugin::ResetServer();
		}

		if (AuralizationModeController)
		{
			AuralizationModeController->Initialize();
		}
		
		// Initialize the dirManager
		DirManager->ResetManager();
		if (bReadInitialMappingFile)
		{
			DirManager->ReadConfigFile(DirMappingFileName);
		}

		// Initialize the HRIRManager
		HRIRManager->ResetManager();

		// Initialize Receiver Actor
		ReceiverID = FVAPlugin::CreateNewSoundReceiver(this);
		if (FVAHRIRManager::GetDefaultHRIR().IsValid() && VA::IsValidID( FVAHRIRManager::GetDefaultHRIR()->GetID() ))
		{
			CurrentHRIR = FVAHRIRManager::GetDefaultHRIR();
		}

	}

	// Initialize Walls for Sound Reflection
	if (!bWallsInitialized)
	{
		InitializeWalls();
	}


	// Handle all sound Sources
	TArray<AActor*> ActorsA;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), ActorsA);

	for (AActor* Actor : ActorsA)
	{
		TArray<UVAAbstractSourceComponent*> VASourceComponents;
		Actor->GetComponents(VASourceComponents);
		for (UVAAbstractSourceComponent* SourceComponent : VASourceComponents)
		{
			SourceComponent->Initialize();
		}
	}

	if (UVirtualRealityUtilities::IsMaster())
	{
		if (FVAPlugin::GetDebugMode())
		{
			RunOnAllNodes("debugMode = true");
		}
		else
		{
			RunOnAllNodes("debugMode = false");
		}
	}

	bInitialized = true;
}

void AVAReceiverActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RunOnAllNodesEvent.Detach();

	if(!FVAPlugin::GetWasStarted())
	{
		return;
	}

	FVAPlugin::ResetServer();

	DirManager->ResetManager();
	HRIRManager->ResetManager();

	Super::EndPlay(EndPlayReason);
}

void AVAReceiverActor::InitializeWalls()
{
	TArray<AActor*> WallsA;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReflectionWall::StaticClass(), WallsA);
	for (AActor* EntryWalls : WallsA)
	{
		ReflectionWalls.Add(static_cast<AVAReflectionWall*>(EntryWalls));
	}
	bWallsInitialized = true;
}

void AVAReceiverActor::SetUpdateRate(const int Rate)
{
	UpdateRate = Rate;
}

// ****************************************************************** // 
// ******* Tick Function ******************************************** //
// ****************************************************************** //

void AVAReceiverActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FVAPlugin::GetUseVA() || !UVirtualRealityUtilities::IsMaster())
	{
		return;
	}

	if (!bInitialized)
	{
		FVAUtils::OpenMessageBox("[AVAReceiverActor::Tick()]: Receiver Actor is not initialized", true);
	}

	TimeSinceUpdate += DeltaTime;
	TotalTime += DeltaTime;

	if (TimeSinceUpdate > (1.0f / float(UpdateRate)))
	{
		UpdateVirtualWorldPose();
		UpdateRealWorldPose();

		TimeSinceUpdate = 0.0f;
	}
}


// ****************************************************************** // 
// ******* Position updates ***************************************** //
// ****************************************************************** //

bool AVAReceiverActor::UpdateVirtualWorldPose()
{
	FVector ViewPos;
	FRotator ViewRot;
	
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(ViewPos, ViewRot);

	FVAPlugin::SetSoundReceiverPosition(ReceiverID, ViewPos);
	FVAPlugin::SetSoundReceiverRotation(ReceiverID, ViewRot);

	return false;
}

bool AVAReceiverActor::UpdateRealWorldPose()
{
	if (!UVirtualRealityUtilities::IsMaster() || !FVAPlugin::GetUseVA())
	{
		return false;
	}

	if (GetWorld() == nullptr)
	{
		return false;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		return false;
	}

	AVirtualRealityPawn* VirtualRealityPawn = dynamic_cast<AVirtualRealityPawn*>(PlayerController->AcknowledgedPawn);
	if (VirtualRealityPawn == nullptr)
	{
		return false;
	}

	USceneComponent* Head	= VirtualRealityPawn->Head;

	if (!Head)
	{
		return false;
	}

	// calculate positions
	const FQuat InverseOriginRot	= VirtualRealityPawn->GetActorQuat().Inverse();
	const FVector Pos				= InverseOriginRot.RotateVector(
		Head->GetComponentLocation() - VirtualRealityPawn->GetActorLocation());
	const FQuat Quat				= InverseOriginRot * Head->GetComponentQuat();

	return FVAPlugin::SetSoundReceiverRealWorldPose(ReceiverID, Pos, Quat.Rotator());
}


// ****************************************************************** // 
// ******* Directivity / HRIR Handling ****************************** //
// ****************************************************************** //

TSharedPtr<FVADirectivity> AVAReceiverActor::GetDirectivityByMapping(const FString Phoneme) const
{
	return DirManager->GetDirectivityByPhoneme(Phoneme);
}

TSharedPtr<FVADirectivity> AVAReceiverActor::GetDirectivityByFileName(const FString FileName)
{
	return DirManager->GetDirectivityByFileName(FileName);
}

bool AVAReceiverActor::ReadDirMappingFile(const FString FileName)
{
	if (DirManager->GetFileName() == FileName)
	{
		FVAUtils::LogStuff("[AVAReceiverActor::ReadDirMappingFile()]: file already loaded", false);
		return false;
	}

	DirMappingFileName = FileName;
	DirManager->ResetManager();
	return DirManager->ReadConfigFile(DirMappingFileName);
}

bool AVAReceiverActor::SetHRIRByFileName(const FString FileName)
{
	TSharedPtr<FVAHRIR> NewHRIR = HRIRManager->GetHRIRByFileName(FileName);
	if (!NewHRIR.IsValid())
	{
		return false;
	}
	
	if(CurrentHRIR.IsValid() && NewHRIR->GetID() == CurrentHRIR->GetID())
	{
		return true;
	}
	
	if (FVAPlugin::SetSoundReceiverHRIR(ReceiverID, NewHRIR->GetID()))
	{
		CurrentHRIR = NewHRIR;
		return true;
	}
	return false;
}


// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

bool AVAReceiverActor::IsInitialized() const
{
	return bInitialized;
}

float AVAReceiverActor::GetScale() const
{
	return WorldScale;
}


FString AVAReceiverActor::GetIPAddress() const
{
	switch (AddressSetting)
	{
	case EConnectionSetting::Automatic:
#if PLATFORM_WINDOWS
		return FString("127.0.0.1");
#else
		return FString("10.0.1.240");
#endif
		break;
	case EConnectionSetting::Cave:
		return FString("10.0.1.240");
		break;
	case EConnectionSetting::Localhost:
		return FString("127.0.0.1");
		break;
	case EConnectionSetting::Manual:
		return ServerIPAddress;
		break;
	default:
		break;
	}

	FVAUtils::LogStuff("[AVAReceiverActor::GetIPAddress()]: Unreachable Error", true);

	return FString("127.0.0.1");
}

int AVAReceiverActor::GetPort() const
{
	switch (AddressSetting)
	{
	case EConnectionSetting::Automatic:
	case EConnectionSetting::Cave:
	case EConnectionSetting::Localhost:
		return 12340;
		break;
	case EConnectionSetting::Manual:
		return ServerPort;
		break;
	default:
		break;
	}

	FVAUtils::LogStuff("[AVAReceiverActor::GetPort()]: Unreachable Error", true);

	return -1;
}

UVAAuralizationModeController* AVAReceiverActor::GetAuralizationModeController() const
{
	return AuralizationModeController;
}

int AVAReceiverActor::GetReceiverID() const
{
	return ReceiverID;
}

int AVAReceiverActor::GetUpdateRate() const
{
	return UpdateRate;
}

TArray<AVAReflectionWall*> AVAReceiverActor::GetReflectionWalls()
{
	if (!bWallsInitialized)
	{
		InitializeWalls();
	}
	return ReflectionWalls;
}



// ****************************************************************** // 
// ******* Cluster Stuff ******************************************** // 
// ****************************************************************** //

void AVAReceiverActor::SetDebugMode(const bool bDebugMode)
{
	if (bDebugMode)
	{
		RunOnAllNodes("debugMode = true");
	}
	else
	{
		RunOnAllNodes("debugMode = false");
	}
}


void AVAReceiverActor::RunOnAllNodes(FString Command)
{
	FVAUtils::LogStuff("[AVAReceiverActor::RunOnAllNodes()]: Cluster Command " + 
		Command + " received", false);
	
	if (Command == "useVA = true")
	{
		FVAPlugin::SetUseVA(true);
	}
	else if (Command == "useVA = false")
	{
		FVAPlugin::SetUseVA(false);
	}
	else if (Command == "debugMode = true")
	{
		FVAPlugin::SetDebugMode(true);
	}
	else if (Command == "debugMode = false")
	{
		FVAPlugin::SetDebugMode(false);
	}
	else
	{
		FVAUtils::LogStuff("[AVAReceiverActor::RunOnAllNodes()]: Cluster Command " + 
			Command + " could not be evaluated.", true);
	}
}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** // 
// ****************************************************************** //

#if WITH_EDITOR
bool AVAReceiverActor::CanEditChange(const FProperty* InProperty) const
{
	// Check manual Address
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, ServerIPAddress) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, ServerPort))
	{
		return AddressSetting == EConnectionSetting::Manual;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, DirMappingFileName))
	{
		return bReadInitialMappingFile;
	}

	return true;
}
#endif
