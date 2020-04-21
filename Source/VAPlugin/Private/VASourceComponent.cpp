// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"


#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VAReceiverActor.h"
#include "VAPlugin.h"

/*
#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "Array.h"
#include "Components/SkeletalMeshComponent.h"
*/

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ReceiverActors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReceiverActor::StaticClass(), ReceiverActors);
	AVAReceiverActor* ReceiverActorTmp = nullptr;

	for (AActor* EntryReceiverActor : ReceiverActors)
	{
		// if there is an ReceiverActor in the Scene
		ReceiverActorTmp = dynamic_cast<AVAReceiverActor*>(EntryReceiverActor);
		if (ReceiverActorTmp != nullptr)
		{
			FVAUtils::LogStuff("[UVASourceComponent::BeginPlay()]: AVAReceiver found");
			break;
		}
	}

	// If no Rec Actor found spawn one with default parameters
	if (ReceiverActorTmp == nullptr)
	{
		FVAUtils::LogStuff("[UVASourceComponent::BeginPlay()]: No AVAReceiver found! Spawning one with default values");
		ReceiverActorTmp = this->GetWorld()->SpawnActor<AVAReceiverActor>(AVAReceiverActor::StaticClass());
	}

	// If the receiver Actor is initialized but this sound Component not, this Component is spawned at runtime and has to be initialized
	if (ReceiverActorTmp->IsInitialized() && !bInitialized)
	{
		Initialize();
	}

	UpdateRate = ReceiverActorTmp->GetUpdateRate();
}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FVAPlugin::GetUseVA())
	{
		return;
	}

	if (!bInitialized)
	{
		FVAUtils::OpenMessageBox("Sound source is not initialized", true);
	}

	if (bFirstTick && FVAPlugin::GetIsMaster())
	{
		TimeSinceUpdate = 1.0f;
		if (StartingPlayAction == Play)
		{
			SoundSource->PlaySoundFromSecond(StartingTime);
		}
	}
	bFirstTick = false;

	TimeSinceUpdate += DeltaTime;

	if ((MovementSetting == AttachToBone || MovementSetting == MoveWithObject) &&
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


	SkeletalMeshComponent = dynamic_cast<USkeletalMeshComponent*>(
		GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (MovementSetting == AttachToBone)
	{
		if (SkeletalMeshComponent != nullptr
			&& SkeletalMeshComponent->DoesSocketExist(FName(*BoneName)))
		{
			FVAUtils::LogStuff("Bone detected.");
		}
		else
		{
			FVAUtils::OpenMessageBox("Error: Could not find bone, using MoveWithObject instead.");
			MovementSetting = MoveWithObject;
		}
	}


	SpawnPosition = GetOwner()->GetTransform().GetLocation();
	SpawnRotation = GetOwner()->GetTransform().GetRotation().Rotator();


	SoundSource = new FVASoundSource(this);

	if (FVAPlugin::GetIsMaster())
	{
		switch (DirectivitySetting)
		{
		case DefaultHRIR:
			SoundSource->SetDirectivity(FVADirectivityManager::GetDefaultDirectivity());
			break;

		case ManualFile:
			SoundSource->SetDirectivity(
				AVAReceiverActor::GetCurrentReceiverActor()->GetDirectivityByFileName(DirectivityByFileName));
			break;

		case Phoneme:
			SoundSource->SetDirectivity(
				AVAReceiverActor::GetCurrentReceiverActor()->GetDirectivityByMapping(DirectivityByMapping));
			break;

		default:
			break;
		}
	}

	FVAUtils::LogStuff("SoundSourceComponent initialized successfully");

	bInitialized = true;
}

bool UVASourceComponent::ShouldSendCommand() const
{
	return (bInitialized && FVAPlugin::GetUseVA() && FVAPlugin::GetIsMaster());
}


// ****************************************************************** // 
// ******* Playback Settings **************************************** //
// ****************************************************************** //

bool UVASourceComponent::PlaySound() const
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	SoundSource->PlaySound();

	return true;
}

bool UVASourceComponent::PlaySoundFromSecond(const float Time) const
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	SoundSource->PlaySoundFromSecond(Time);

	return true;
}

bool UVASourceComponent::StopSound() const
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	SoundSource->StopSound();

	return true;
}

bool UVASourceComponent::PauseSound() const
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	SoundSource->PauseSound();

	return true;
}

EPlayAction UVASourceComponent::GetPlayState() const
{
	if (!ShouldSendCommand())
	{
		return Stop;
	}

	return FVAUtils::VAActionToEPlayAction(SoundSource->GetPlayState());
}



// ****************************************************************** // 
// ******* Sound Settings ******************************************* //
// ****************************************************************** //

bool UVASourceComponent::MuteSound(const bool bMuteN)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	if (bMuted == bMuteN)
	{
		return true;
	}

	bMuted = bMuteN;

	SoundSource->MuteSound(bMuted);

	return true;
}

bool UVASourceComponent::LoadSoundFile(FString SoundFileN) const
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	return SoundSource->LoadNewSound(SoundFile);
}

bool UVASourceComponent::SetSoundFile(const FString SoundFileN)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	// If already playing back that sound File
	if (SoundFile == SoundFileN)
	{
		SoundSource->StopSound();
		return true;
	}

	SoundFile = SoundFileN;

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

	// If already playing back that sound File
	if (SoundPower == Power)
	{
		return true;
	}

	SoundPower = Power;

	SoundSource->SetPower(Power);

	return true;
}

float UVASourceComponent::GetSoundPower() const
{
	return SoundPower;
}

bool UVASourceComponent::SetLoop(const bool bLoopN)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	// If this setting is already set properly
	if (bLoop == bLoopN)
	{
		return true;
	}

	bLoop = bLoopN;

	SoundSource->SetLoop(bLoopN);

	return true;
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
	case MoveWithObject:
		Pos = GetOwner()->GetTransform().GetLocation();
		break;

	case ObjectSpawnPoint:
		Pos = SpawnPosition;
		break;

	case AttachToBone:
		Pos = SkeletalMeshComponent->GetSocketLocation(FName(*BoneName));
		break;

	default:
		Pos = FVector::ZeroVector;
		FVAUtils::LogStuff(FString("[UVASourceComponent::getPosition()]: default"));
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
	case MoveWithObject:
		Rot = GetOwner()->GetTransform().GetRotation().Rotator();
		break;

	case ObjectSpawnPoint:
		Rot = SpawnRotation;
		break;

	case AttachToBone:
		Rot = SkeletalMeshComponent->GetSocketRotation(FName(*BoneName));
		break;

	default:
		FVAUtils::LogStuff(FString("[UVASourceComponent::getRotation()]: default"));
		Rot = FRotator::ZeroRotator;
		break;
	}

	if (bUsePoseOffset)
	{
		Rot = FRotator(FQuat(Rot) * FQuat(OffsetRotation));
	}

	return Rot;
}

bool UVASourceComponent::SetMovementSetting(const EMovement MovementSettingN)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (MovementSetting == MovementSettingN)
	{
		return true;
	}

	MovementSetting = MovementSettingN;


	SoundSource->SetPosition(GetPosition());
	SoundSource->SetRotation(GetRotation());

	return true;
}

bool UVASourceComponent::SetUsePoseOffset(const bool bUsePoseOffsetN)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (bUsePoseOffset == bUsePoseOffsetN)
	{
		return true;
	}

	bUsePoseOffset = bUsePoseOffsetN;

	SoundSource->SetPosition(GetPosition());
	SoundSource->SetRotation(GetRotation());

	return true;
}

bool UVASourceComponent::SetOffsetPosition(const FVector PosN)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (OffsetPosition == PosN)
	{
		return true;
	}

	OffsetPosition = PosN;

	SoundSource->SetPosition(GetPosition());

	return true;
}

bool UVASourceComponent::SetOffsetRotation(const FRotator RotN)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (OffsetRotation == RotN)
	{
		return true;
	}

	OffsetRotation = RotN;

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

	DirectivitySetting = EDir::Phoneme;
	DirectivityByMapping = Phoneme;

	SoundSource->SetDirectivity(AVAReceiverActor::GetCurrentReceiverActor()->GetDirectivityByMapping(Phoneme));

	return true;
}

bool UVASourceComponent::SetDirectivityByFileName(const FString FileName)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	DirectivitySetting = ManualFile;
	DirectivityByFileName = FileName;

	SoundSource->SetDirectivity(AVAReceiverActor::GetCurrentReceiverActor()->GetDirectivityByFileName(FileName));

	return true;
}

FString UVASourceComponent::GetDirectivityFileName() const
{
	if (SoundSource != nullptr)
	{
		return SoundSource->GetDirectivity()->GetFileName();
	}

	return "";
}



// ****************************************************************** // 
// ******* Graphical Representation ********************************* //
// ****************************************************************** //

bool UVASourceComponent::SetVisibility(const bool bVisN) const
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	SoundSource->SetVisibility(bVisN);

	return true;
}

bool UVASourceComponent::GetVisibility() const
{
	return SoundSource->GetVisibility();
}

bool UVASourceComponent::SetBoneName(const FString BoneNameN)
{
	if (!FVAPlugin::GetUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (BoneName == BoneNameN)
	{
		return true;
	}

	BoneName = BoneNameN;

	// Check if the bone exists
	if (SkeletalMeshComponent != nullptr
		&& SkeletalMeshComponent->DoesSocketExist(FName(*BoneNameN)))
	{
		BoneName = BoneNameN;
		MovementSetting = AttachToBone;
		return true;
	}
	FVAUtils::OpenMessageBox(
		"[UVASourceComponent::setBoneName(FString)]: Could not find new bone, using old settings instead.");
	return false;
}

FString UVASourceComponent::GetBoneName() const
{
	if (MovementSetting != AttachToBone)
	{
		FVAUtils::LogStuff("[UVASourceComponent::getBoneName()]: Movement is not set to AttatchToBone..");
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
		return MovementSetting == AttachToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, DirectivityByFileName))
	{
		return DirectivitySetting == ManualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, DirectivityByMapping))
	{
		return DirectivitySetting == Phoneme;
	}


	return ParentVal;
}
#endif
