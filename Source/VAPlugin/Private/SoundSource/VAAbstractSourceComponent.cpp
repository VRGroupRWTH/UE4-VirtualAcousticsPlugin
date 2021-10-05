// Fill out your copyright notice in the Description page of Project Settings.

#include "SoundSource/VAAbstractSourceComponent.h"

#include "SoundSource/VASoundSource.h"
#include "VAReceiverActor.h"
#include "VAPlugin.h"
#include "VAUtils.h"

#include "ImageSourceModel/VAImageSourceModel.h"

#include "SignalSources/VAAudiofileSignalSource.h"
#include "SignalSources/VAJetEngineSignalSource.h"

#include "Components/SkeletalMeshComponent.h"		// Skeletons
#include "Kismet/GameplayStatics.h"					// Get Actors of Class


// Sets default values for this component's properties
UVAAbstractSourceComponent::UVAAbstractSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//Default objects for subcomponents does not work
	//SignalSource = CreateDefaultSubobject<UVAAudiofileSignalSource>(TEXT("SignalSource"), false);

  // If the receiver Actor is initialized but this sound Component not, this Component is spawned at runtime and has to be initialized
  // Otherwise it will be later on initialized from the Receiver Actor
  TArray<AActor*> ReceiverActors;
  UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReceiverActor::StaticClass(), ReceiverActors);
  if (ReceiverActors.Num() >= 1 && Cast<AVAReceiverActor>(ReceiverActors[0])->IsInitialized() && !bInitialized)
  {
    Initialize();
  }
}

// Called when the game starts
void UVAAbstractSourceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVAAbstractSourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SoundSource.Reset();
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UVAAbstractSourceComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FVAPlugin::GetUseVA() || !FVAPlugin::GetIsMaster())
	{
		return;
	}

	if (!bInitialized)
	{
	    FVAUtils::OpenMessageBox("[UVASourceComponent::TickComponent()]: Sound source is not initialized", true);
		return;
	}


	if (bFirstTick && FVAPlugin::GetIsMaster())
	{
		TimeSinceUpdate = 1.0f;
	}
	bFirstTick = false;

	TimeSinceUpdate += DeltaTime;

	if ((MovementSetting == EMovement::AttachToBone || MovementSetting == EMovement::MoveWithObject) &&
		TimeSinceUpdate > (1.0f / float(UpdateRate)))
	{
		UpdatePose();

		TimeSinceUpdate = 0.0f;
	}
}


void UVAAbstractSourceComponent::Initialize()
{
	if (!FVAPlugin::GetUseVA() || bInitialized)
	{
		return;
	}

	bFirstTick = true;


	TArray<AActor*> ReceiverActors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReceiverActor::StaticClass(), ReceiverActors);
	AVAReceiverActor* ReceiverActorTmp = nullptr;

	if (ReceiverActors.Num() == 0)
	{
		// If no Rec Actor found spawn one with default parameters
		if (ReceiverActorTmp == nullptr)
		{
			FVAUtils::LogStuff("[UVASourceComponent::BeginPlay()]: No AVAReceiver found! Spawning one with default values", false);
			ReceiverActorTmp = this->GetWorld()->SpawnActor<AVAReceiverActor>(AVAReceiverActor::StaticClass());
		}
	}
	else if (ReceiverActors.Num() == 1)
	{
		ReceiverActorTmp = dynamic_cast<AVAReceiverActor*>(ReceiverActors[0]);
		FVAUtils::LogStuff("[UVASourceComponent::BeginPlay()]: Receiver found!", false);
	}
	else if (ReceiverActors.Num() >= 2)
	{
		FVAPlugin::SetUseVA(false);

		FVAUtils::OpenMessageBox("There are more than 1 Receiver Actors in the world. Stopping VAPlugin. Make sure to remove the wrong one.", true);
		FVAUtils::LogStuff("[UVASourceComponent::BeginPlay()]: More than 1 Receiver found! Stopping VAPlugin. Make sure to remove the wrong one", true);

		return;
	}

	CurrentReceiverActor = ReceiverActorTmp;

	UpdateRate = ReceiverActorTmp->GetUpdateRate();

	

	SkeletalMeshComponent = dynamic_cast<USkeletalMeshComponent*>(
		GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (MovementSetting == EMovement::AttachToBone)
	{
		if (SkeletalMeshComponent != nullptr
			&& SkeletalMeshComponent->DoesSocketExist(FName(*BoneName)))
		{
			FVAUtils::LogStuff("[UVASourceComponent::Initialize()]: Bone " + BoneName + " detected.", false);
		}
		else
		{
			FVAUtils::OpenMessageBox("[UVASourceComponent::Initialize()]: Could not find bone " + 
				BoneName  + ", using MoveWithObject instead.", true);
			MovementSetting = EMovement::MoveWithObject;
		}
	}


	SpawnPosition = GetOwner()->GetTransform().GetLocation();
	SpawnRotation = GetOwner()->GetTransform().GetRotation().Rotator();

	const std::string SoundSourceName = std::string( TCHAR_TO_UTF8(*GetName()) );
	SoundSource = MakeShared<FVASoundSource>(GetWorld(), GetPosition(), GetRotation(), SoundPower, SoundSourceName);

	if (FVAPlugin::GetIsMaster())
	{
		switch (DirectivitySetting)
		{
		case EDirectivitySetting::DefaultDirectivity:
			SoundSource->SetDirectivity(FVADirectivityManager::GetDefaultDirectivity());
			break;

		case EDirectivitySetting::ManualFile:
			SoundSource->SetDirectivity(
				CurrentReceiverActor->GetDirectivityByFileName(DirectivityByFileName));
			break;

		case EDirectivitySetting::Phoneme:
			SoundSource->SetDirectivity(
				CurrentReceiverActor->GetDirectivityByMapping(DirectivityByMapping));
			break;

		default:
			break;
		}
	}

	if (bHandleReflections)
	{
		TArray<AVAReflectionWall*> ReflWalls = CurrentReceiverActor->GetReflectionWalls();
		ImageSourceModel = MakeShared<FVAImageSourceModel>(GetWorld(), SoundSource.ToSharedRef(), ReflWalls);
	}

	if (SignalSource)
	{
		SignalSource->Initialize();
		SetSignalSourceID(SignalSource->GetID());
	}

	FVAUtils::LogStuff("[UVASourceComponent::Initialize()]: SoundSourceComponent initialized successfully", false);

	bInitialized = true;
}

void UVAAbstractSourceComponent::UpdatePose()
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return;
	}

	SoundSource->SetPosition(GetPosition());
	SoundSource->SetRotation(GetRotation());
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		ImageSourceModel->UpdateISPositions();
		ImageSourceModel->UpdateISRotations();
	}
}


bool UVAAbstractSourceComponent::SetSignalSourceID(const std::string& SignalSourceID)
{
	if (!SoundSource.IsValid())
	{
		FVAUtils::OpenMessageBox(FString("[UVASourceComponent::SetSignalSourceID]: VA sound source not initialized"), true);
		return false;
	}
	if (!SoundSource->SetSignalSource(SignalSourceID))
	{
		return false;
	}
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		return ImageSourceModel->UpdateISSignalSource();
	}
	return true;
}

bool UVAAbstractSourceComponent::ShouldSendCommand() const
{
	return (bInitialized && FVAPlugin::GetUseVA() && FVAPlugin::GetIsMaster());
}


// ****************************************************************** // 
// ******* Sound Source Settings ************************************ //
// ****************************************************************** //

bool UVAAbstractSourceComponent::MuteSound(const bool bMute)
{
	if (!ShouldSendCommand())
	{
		return false;
	}
	if (!SoundSource->MuteSound(bMute))
	{
		return false;
	}
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		return ImageSourceModel->MuteIS(bMute);
	}
	return true;
}


bool UVAAbstractSourceComponent::SetSoundPower(const float Power)
{
	if (!ShouldSendCommand())
	{
		return false;
	}
	if (!SoundSource->SetPower(Power))
	{
		return false;
	}
	SoundPower = Power;
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		return ImageSourceModel->UpdateISPower();
	}
	return true;
}

float UVAAbstractSourceComponent::GetSoundPower() const
{
	return SoundPower;
}

// ****************************************************************** // 
// ******* Image Sources / Reflections ****************************** //
// ****************************************************************** //

bool UVAAbstractSourceComponent::GetHandleReflections() const
{
	return bHandleReflections;
}



// ****************************************************************** // 
// ******* Sound Pose *********************************************** //
// ****************************************************************** //

FVector UVAAbstractSourceComponent::GetPosition() const
{
	FVector Pos;
	switch (MovementSetting)
	{
	case EMovement::MoveWithObject:
		Pos = GetOwner()->GetTransform().GetLocation();
		break;

	case EMovement::ObjectSpawnPoint:
		Pos = SpawnPosition;
		break;

	case EMovement::AttachToBone:
		Pos = SkeletalMeshComponent->GetSocketLocation(FName(*BoneName));
		break;

	default:
		Pos = FVector::ZeroVector;
		FVAUtils::LogStuff(FString("[UVASourceComponent::GetPosition()]: In default, Unreachable Error", true));
		break;
	}

	if (bUsePoseOffset)
	{
		Pos = Pos + OffsetPosition;
	}

	return Pos;
}

FRotator UVAAbstractSourceComponent::GetRotation() const
{
	FRotator Rot;
	switch (MovementSetting)
	{
	case EMovement::MoveWithObject:
		Rot = GetOwner()->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::ObjectSpawnPoint:
		Rot = SpawnRotation;
		break;

	case EMovement::AttachToBone:
		Rot = SkeletalMeshComponent->GetSocketRotation(FName(*BoneName));
		break;

	default:
		FVAUtils::LogStuff(FString("[UVASourceComponent::GetRotation()]: In default, Unreachable Error", true));
		Rot = FRotator::ZeroRotator;
		break;
	}

	if (bUsePoseOffset)
	{
		Rot = FRotator(FQuat(Rot) * FQuat(OffsetRotation));
	}

	return Rot;
}

bool UVAAbstractSourceComponent::SetMovementSetting(const EMovement::Type NewMovementSetting)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	if (MovementSetting == NewMovementSetting)
	{
		return true;
	}

	MovementSetting = NewMovementSetting;
	UpdatePose();

	return true;
}

bool UVAAbstractSourceComponent::SetUsePoseOffset(const bool bNewUsePoseOffset)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	if (bUsePoseOffset == bNewUsePoseOffset)
	{
		return true;
	}

	bUsePoseOffset = bNewUsePoseOffset;
	UpdatePose();

	return true;
}

bool UVAAbstractSourceComponent::SetOffsetPosition(const FVector Pos)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}
	
	if (OffsetPosition == Pos && bUsePoseOffset)
	{
		return true;
	}

	bUsePoseOffset = true;
	OffsetPosition = Pos;
	UpdatePose();

	return true;
}

bool UVAAbstractSourceComponent::SetOffsetRotation(const FRotator Rot)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	if (OffsetRotation == Rot && bUsePoseOffset)
	{
		return true;
	}
	bUsePoseOffset = true;
	OffsetRotation = Rot;
	UpdatePose();

	return true;
}



// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //

bool UVAAbstractSourceComponent::SetDirectivityByMapping(const FString Phoneme)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	DirectivitySetting = EDirectivitySetting::Phoneme;
	DirectivityByMapping = Phoneme;

	return SoundSource->SetDirectivity(CurrentReceiverActor->GetDirectivityByMapping(Phoneme));
}

bool UVAAbstractSourceComponent::SetDirectivityByFileName(const FString FileName)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	DirectivitySetting = EDirectivitySetting::ManualFile;
	DirectivityByFileName = FileName;

	if (FileName == "")
	{
		 return SoundSource->RemoveDirectivity();
	}
	
	return SoundSource->SetDirectivity(CurrentReceiverActor->GetDirectivityByFileName(FileName));
}

FString UVAAbstractSourceComponent::GetDirectivityFileName() const
{
	if (SoundSource.IsValid())
	{
		return SoundSource->GetDirectivityFilename();
	}

	return "";
}



// ****************************************************************** // 
// ******* Graphical Representation ********************************* //
// ****************************************************************** //

bool UVAAbstractSourceComponent::SetVisibility(const bool bVis)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	SoundSource->SetVisibility(bVis);
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		ImageSourceModel->SetISVisibility(bVis);
	}
	return true;
}

bool UVAAbstractSourceComponent::GetVisibility() const
{
	return SoundSource->GetVisibility();
}

int UVAAbstractSourceComponent::GetSoundSourceID() const
{
	if (!SoundSource)
	{
		return -1;
	}
	return SoundSource->GetSoundSourceID();
}

bool UVAAbstractSourceComponent::SetBoneName(const FString NewBoneName)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	if (BoneName == NewBoneName)
	{
		return true;
	}

	// Check if the bone exists
	if (SkeletalMeshComponent != nullptr
		&& SkeletalMeshComponent->DoesSocketExist(FName(*NewBoneName)))
	{
		BoneName = NewBoneName;
		MovementSetting = EMovement::AttachToBone;
		FVAUtils::LogStuff("[UVASourceComponent::SetBoneName()]: Successfully found bone with name" + 
			BoneName + "and set Movement Setting to follow the bone", false);
		return true;
	}
	FVAUtils::OpenMessageBox(
		"[UVASourceComponent::SetBoneName()]: Could not find the bone, using old settings instead.");
	return false;
}

FString UVAAbstractSourceComponent::GetBoneName() const
{
	if (MovementSetting != EMovement::AttachToBone)
	{
		FVAUtils::LogStuff("[UVASourceComponent::GetBoneName()]: Movement is not set to AttachToBone..", true);
		return "SoundSource is not attached to bone, but current bone is: " + BoneName;
	}
	
	return BoneName;
}


// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR

bool UVAAbstractSourceComponent::CanEditChange(const FProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVAAbstractSourceComponent, BoneName))
	{
		return MovementSetting == EMovement::AttachToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVAAbstractSourceComponent, DirectivityByFileName))
	{
		return DirectivitySetting == EDirectivitySetting::ManualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVAAbstractSourceComponent, DirectivityByMapping))
	{
		return DirectivitySetting == EDirectivitySetting::Phoneme;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVAAbstractSourceComponent, OffsetPosition) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVAAbstractSourceComponent, OffsetRotation))
	{
		return bUsePoseOffset;
	}


	return ParentVal;
}
#endif
