// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "VASourceComponent.h"
#include "VAReflectionWall.h"


#include "Engine/World.h"							// World
#include "GameFramework/PlayerController.h"			// Viewport
#include "IDisplayCluster.h"						// For Events
#include "VirtualRealityPawn.h"						// VR Pawn
#include "Kismet/GameplayStatics.h"					// Get Actors of class



// ****************************************************************** // 
// ******* Initialization Functions ********************************* //
// ****************************************************************** //

AVAReceiverActor::AVAReceiverActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVAReceiverActor::BeginPlay()
{
	Super::BeginPlay();

	FVAPlugin::SetReceiverActor(this);

	// CurrentReceiverActor = this;

	//try to start (remote) VAServer automatically
	bool bStartedVAServer = false;
	if (bAutomaticRemoteVAStart)
	{
		bStartedVAServer = FVAPlugin::RemoteStartVAServer(GetIPAddress(), RemoteVAStarterPort,
			WhichVAServerVersionToStart);
	}
	
	// Ask if used or not
	FVAPlugin::AskForSettings(GetIPAddress(), GetPort(), bAskForDebugMode, !bStartedVAServer);

	if (FVAPlugin::GetIsMaster())
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

	// Cluster Stuff for Events //
	IDisplayClusterClusterManager* ClusterManager = IDisplayCluster::Get().GetClusterMgr();
	if (ClusterManager && !ClusterEventListenerDelegate.IsBound())
	{
		ClusterEventListenerDelegate = FOnClusterEventListener::CreateUObject(
			this, &AVAReceiverActor::HandleClusterEvent);
		ClusterManager->AddClusterEventListener(ClusterEventListenerDelegate);
	}

	if (!FVAPlugin::GetUseVA())
	{
		return;
	}


	if (FVAPlugin::GetIsMaster())
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
		
		// Initialize the dirManager
		DirManager.ResetManager();
		if (bReadInitialMappingFile)
		{
			DirManager.ReadConfigFile(DirMappingFileName);
		}

		// Initialize the HRIRManager
		HRIRManager.ResetManager();

		// Initialize Receiver Actor
		ReceiverID = FVAPlugin::CreateNewSoundReceiver(this);
		if (FVAHRIRManager::GetDefaultHRIR()->GetID() != -1)
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

	for (AActor* EntryActor : ActorsA)
	{
		TArray<UActorComponent*> VASourceComponents = EntryActor->GetComponentsByClass(UVASourceComponent::StaticClass());
		for (UActorComponent* EntrySourceComponent : VASourceComponents)
		{
			Cast<UVASourceComponent>(EntrySourceComponent)->Initialize();
		}
	}

	if (FVAPlugin::GetIsMaster())
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

void AVAReceiverActor::BeginDestroy()
{
	Super::BeginDestroy();

	FVAPlugin::ResetServer();

	DirManager.ResetManager();
	HRIRManager.ResetManager();

	IDisplayClusterClusterManager* ClusterManager = IDisplayCluster::Get().GetClusterMgr();
	if (ClusterManager && ClusterEventListenerDelegate.IsBound())
	{
		ClusterManager->RemoveClusterEventListener(ClusterEventListenerDelegate);
	}
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

	if (!FVAPlugin::GetUseVA() || !FVAPlugin::GetIsMaster())
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
	if (!(FVAPlugin::GetIsMaster() && FVAPlugin::GetUseVA()))
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

	USceneComponent* Head	= VirtualRealityPawn->GetHeadComponent();
	USceneComponent* Origin = VirtualRealityPawn->GetTrackingOriginComponent();

	if (!Head || !Origin)
	{
		return false;
	}

	// calculate positions
	const FQuat InverseOriginRot	= Origin->GetComponentQuat().Inverse();
	const FVector Pos				= InverseOriginRot.RotateVector(
		Head->GetComponentLocation() - Origin->GetComponentLocation());
	const FQuat Quat				= InverseOriginRot * Head->GetComponentQuat();

	return FVAPlugin::SetSoundReceiverRealWorldPose(ReceiverID, Pos, Quat.Rotator());
}


// ****************************************************************** // 
// ******* Directivity / HRIR Handling ****************************** //
// ****************************************************************** //

FVADirectivity* AVAReceiverActor::GetDirectivityByMapping(const FString Phoneme) const
{
	return DirManager.GetDirectivityByPhoneme(Phoneme);
}

FVADirectivity* AVAReceiverActor::GetDirectivityByFileName(const FString FileName)
{
	return DirManager.GetDirectivityByFileName(FileName);
}

bool AVAReceiverActor::ReadDirMappingFile(const FString FileName)
{
	if (DirManager.GetFileName() == FileName)
	{
		FVAUtils::LogStuff("[AVAReceiverActor::ReadDirMappingFile()]: file already loaded", false);
		return false;
	}

	DirMappingFileName = FileName;
	DirManager.ResetManager();
	return DirManager.ReadConfigFile(DirMappingFileName);
}

bool AVAReceiverActor::SetHRIRByFileName(const FString FileName)
{
	FVAHRIR* NewHRIR = HRIRManager.GetHRIRByFileName(FileName);

	if (NewHRIR == nullptr)
	{
		return false;
	}
	
	if(CurrentHRIR != nullptr && NewHRIR->GetID() == CurrentHRIR->GetID())
	{
		return true;
	}
	
	CurrentHRIR = NewHRIR;
	return FVAPlugin::SetSoundReceiverHRIR(ReceiverID, NewHRIR->GetID());
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
	case EAddress::Automatic:
#if PLATFORM_WINDOWS
		return FString("127.0.0.1");
#else
		return FString("10.0.1.240");
#endif
		break;
	case EAddress::Cave:
		return FString("10.0.1.240");
		break;
	case EAddress::Localhost:
		return FString("127.0.0.1");
		break;
	case EAddress::Manual:
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
	case EAddress::Automatic:
	case EAddress::Cave:
	case EAddress::Localhost:
		return 12340;
		break;
	case EAddress::Manual:
		return ServerPort;
		break;
	default:
		break;
	}

	FVAUtils::LogStuff("[AVAReceiverActor::GetPort()]: Unreachable Error", true);

	return -1;
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

void AVAReceiverActor::SetDebugMode(const bool bDebugModeN)
{
	if (bDebugModeN)
	{
		RunOnAllNodes("debugMode = true");
	}
	else
	{
		RunOnAllNodes("debugMode = false");
	}
}


void AVAReceiverActor::RunOnAllNodes(const FString Command)
{
	IDisplayClusterClusterManager* const Manager = IDisplayCluster::Get().GetClusterMgr();
	if (Manager)
	{
		if (Manager->IsStandalone())
		{
			//in standalone (e.g., desktop editor play) cluster events are not executed....
			HandleClusterCommand(Command);
			FVAUtils::LogStuff("[AVAReceiverActor::RunOnAllNodes()]: Cluster Command " + 
				Command + " ran local", false);
		}
		else
		{
			// else create a cluster event to react to
			FDisplayClusterClusterEvent ClusterEvent;
			ClusterEvent.Name		= Command;
			ClusterEvent.Category	= "VAPlugin";
			ClusterEvent.Type		= "command";
			
			Manager->EmitClusterEvent(ClusterEvent, true);

			FVAUtils::LogStuff("[AVAReceiverActor::RunOnAllNodes()]: Cluster Command " + 
				Command + " sent", false);
		}
	}
}

void AVAReceiverActor::HandleClusterEvent(const FDisplayClusterClusterEvent& Event)
{
	if (Event.Category == "VAPlugin" && Event.Type == "command")
	{
		HandleClusterCommand(Event.Name);
	}
}

void AVAReceiverActor::HandleClusterCommand(const FString Command)
{
	FVAUtils::LogStuff("[AVAReceiverActor::HandleClusterCommand()]: Cluster Command " + 
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
		FVAUtils::LogStuff("[AVAReceiverActor::HandleClusterCommand()]: Cluster Command " + 
			Command + " could not be evaluated.", true);
	}
}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** // 
// ****************************************************************** //

#if WITH_EDITOR
bool AVAReceiverActor::CanEditChange(const UProperty* InProperty) const
{
	// Check manual Address
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, ServerIPAddress) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, ServerPort))
	{
		return AddressSetting == EAddress::Manual;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, DirMappingFileName))
	{
		return bReadInitialMappingFile;
	}

	return true;
}
#endif
