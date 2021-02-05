// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"

#include "VASoundSource.h"
#include "VAReceiverActor.h"
#include "VAPlugin.h"
#include "VAUtils.h"

#include "Components/SkeletalMeshComponent.h"		// Skeletons
#include "Kismet/GameplayStatics.h"					// Get Actors of Class


// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

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
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UVASourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SoundSource.Reset();
}


// Called every frame
void UVASourceComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
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
  }


	if (bFirstTick && FVAPlugin::GetIsMaster())
	{
		TimeSinceUpdate = 1.0f;
		if (StartingPlayAction == EPlayAction::Play)
		{
			SoundSource->PlaySoundFromSecond(StartingTime);
		}
	}
	bFirstTick = false;

	TimeSinceUpdate += DeltaTime;

	if ((MovementSetting == EMovement::AttachToBone || MovementSetting == EMovement::MoveWithObject) &&
		TimeSinceUpdate > (1.0f / float(UpdateRate)))
	{
		SoundSource->SetPosition(GetPosition());
		SoundSource->SetRotation(GetRotation());

		TimeSinceUpdate = 0.0f;
	}
}


void UVASourceComponent::Initialize()
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

	TArray<AVAReflectionWall*> WallArray = CurrentReceiverActor->GetReflectionWalls();
	SoundSource = MakeShared<FVASoundSource>(this, WallArray);
	if (FVAPlugin::GetIsMaster())
	{
		switch (DirectivitySetting)
		{
		case EDirectivitySetting::DefaultHRIR:
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

	FVAUtils::LogStuff("[UVASourceComponent::Initialize()]: SoundSourceComponent initialized successfully", false);

	bInitialized = true;
}

bool UVASourceComponent::ShouldSendCommand() const
{
	return (bInitialized && FVAPlugin::GetUseVA() && FVAPlugin::GetIsMaster());
}


// ****************************************************************** // 
// ******* Playback Settings **************************************** //
// ****************************************************************** //

bool UVASourceComponent::PlaySound()
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->PlaySound();
}

bool UVASourceComponent::PlaySoundFromSecond(const float Time)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->PlaySoundFromSecond(Time);
}

bool UVASourceComponent::StopSound()
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->StopSound();
}

bool UVASourceComponent::PauseSound()
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->PauseSound();
}

EPlayAction::Type UVASourceComponent::GetPlayState() const
{
	if (!ShouldSendCommand())
	{
		return EPlayAction::Stop;
	}

	return EPlayAction::Type(SoundSource->GetPlayState());
}



// ****************************************************************** // 
// ******* Sound Settings ******************************************* //
// ****************************************************************** //

bool UVASourceComponent::MuteSound(const bool bMute)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	if (bMuted == bMute)
	{
		return true;
	}

	bMuted = bMute;

	return SoundSource->MuteSound(bMuted);
}

bool UVASourceComponent::LoadSoundFile(FString SoundFileName)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->LoadNewSound(SoundFileName);
}

bool UVASourceComponent::SetSoundFile(const FString SoundFileName)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	if (SoundFile == SoundFileName)
	{
		if(SoundSource->StopSound())
		{
			return true;
		}
	}

	SoundFile = SoundFileName;

	return SoundSource->SetNewSound(SoundFile);
}

FString UVASourceComponent::GetSoundFile() const
{
	return SoundFile;
}

bool UVASourceComponent::SetSoundPower(const float Power)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	if (SoundPower == Power)
	{
		return true;
	}

	SoundPower = Power;

	return SoundSource->SetPower(Power);
}

float UVASourceComponent::GetSoundPower() const
{
	return SoundPower;
}

bool UVASourceComponent::SetLoop(const bool bNewLoop)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	if (bLoop == bNewLoop)
	{
		return true;
	}

	bLoop = bNewLoop;

	return SoundSource->SetLoop(bNewLoop);
}

bool UVASourceComponent::GetLoop() const
{
	return bLoop;
}

bool UVASourceComponent::GetHandleReflections() const
{
	return bHandleReflections;
}



// ****************************************************************** // 
// ******* Sound Pose *********************************************** //
// ****************************************************************** //

FVector UVASourceComponent::GetPosition() const
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

FRotator UVASourceComponent::GetRotation() const
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

bool UVASourceComponent::SetMovementSetting(const EMovement::Type NewMovementSetting)
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

	SoundSource->SetPosition(GetPosition());
	SoundSource->SetRotation(GetRotation());

	return true;
}

bool UVASourceComponent::SetUsePoseOffset(const bool bNewUsePoseOffset)
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

	SoundSource->SetPosition(GetPosition());
	SoundSource->SetRotation(GetRotation());

	return true;
}

bool UVASourceComponent::SetOffsetPosition(const FVector Pos)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	if (!bUsePoseOffset)
	{
		bUsePoseOffset = true;
		SoundSource->SetRotation(GetRotation());
	}
	
	if (OffsetPosition == Pos)
	{
		return true;
	}

	OffsetPosition = Pos;

	SoundSource->SetPosition(GetPosition());
	return true;
}

bool UVASourceComponent::SetOffsetRotation(const FRotator Rot)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}
	
	if (!bUsePoseOffset)
	{
		bUsePoseOffset = true;
		SoundSource->SetPosition(GetPosition());
	}

	if (OffsetRotation == Rot)
	{
		return true;
	}

	OffsetRotation = Rot;

	SoundSource->SetRotation(GetRotation());
	return true;
}



// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //

bool UVASourceComponent::SetDirectivityByMapping(const FString Phoneme)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	DirectivitySetting = EDirectivitySetting::Phoneme;
	DirectivityByMapping = Phoneme;

	return SoundSource->SetDirectivity(CurrentReceiverActor->GetDirectivityByMapping(Phoneme));
}

bool UVASourceComponent::SetDirectivityByFileName(const FString FileName)
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

FString UVASourceComponent::GetDirectivityFileName() const
{
	if (SoundSource.IsValid())
	{
		return SoundSource->GetDirectivity()->GetFileName();
	}

	return "";
}



// ****************************************************************** // 
// ******* Graphical Representation ********************************* //
// ****************************************************************** //

bool UVASourceComponent::SetVisibility(const bool bVis)
{
	if (!FVAPlugin::GetUseVA() || !SoundSource.IsValid())
	{
		return false;
	}

	SoundSource->SetVisibility(bVis);
	return true;
}

bool UVASourceComponent::GetVisibility() const
{
	return SoundSource->GetVisibility();
}

bool UVASourceComponent::SetBoneName(const FString NewBoneName)
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

FString UVASourceComponent::GetBoneName() const
{
	if (MovementSetting != EMovement::AttachToBone)
	{
		FVAUtils::LogStuff("[UVASourceComponent::GetBoneName()]: Movement is not set to AttachToBone..", true);
		return "SoundSource is not attached to bone, but current bone is: " + BoneName;
	}
	
	return BoneName;
}



float UVASourceComponent::GetSoundTimeOffset() const
{
	return StartingTime;
}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR
bool UVASourceComponent::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, BoneName))
	{
		return MovementSetting == EMovement::AttachToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, DirectivityByFileName))
	{
		return DirectivitySetting == EDirectivitySetting::ManualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, DirectivityByMapping))
	{
		return DirectivitySetting == EDirectivitySetting::Phoneme;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, OffsetPosition) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, OffsetRotation))
	{
		return bUsePoseOffset;
	}


	return ParentVal;
}
#endif
