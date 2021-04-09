// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"

#include "SoundSource/VASoundSourceBase.h"
#include "VAReceiverActor.h"
#include "VAPlugin.h"
#include "VAUtils.h"

#include "ImageSourceModel/VAImageSourceModel.h"

#include "SignalSources/VAAudiofileSignalSource.h"
#include "SignalSources/VAJetEngineSignalSource.h"

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

void UVASourceComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	ForceUpdateSignalSourceType(SignalSourceType);
}

// Called when the game starts
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();
	BindSignalSourceEvents();
}

void UVASourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SoundSourceBase.Reset();
	UnbindSignalSourceEvents();
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

	const std::string SoundSourceName = std::string( TCHAR_TO_UTF8(*GetName()) );
	SoundSourceBase = MakeShared<FVASoundSourceBase>(GetWorld(), GetPosition(), GetRotation(), SoundPower, SoundSourceName);

	if (FVAPlugin::GetIsMaster())
	{
		switch (DirectivitySetting)
		{
		case EDirectivitySetting::DefaultDirectivity:
			SoundSourceBase->SetDirectivity(FVADirectivityManager::GetDefaultDirectivity());
			break;

		case EDirectivitySetting::ManualFile:
			SoundSourceBase->SetDirectivity(
				CurrentReceiverActor->GetDirectivityByFileName(DirectivityByFileName));
			break;

		case EDirectivitySetting::Phoneme:
			SoundSourceBase->SetDirectivity(
				CurrentReceiverActor->GetDirectivityByMapping(DirectivityByMapping));
			break;

		default:
			break;
		}
	}

	if (bHandleReflections)
	{
		TArray<AVAReflectionWall*> ReflWalls = CurrentReceiverActor->GetReflectionWalls();
		ImageSourceModel = MakeShared<FVAImageSourceModel>(GetWorld(), SoundSourceBase.Get(), ReflWalls);
	}

	if (SignalSource)
	{
		SignalSource->Initialize();
		SetSignalSourceID(SignalSource->GetID());
	}

	FVAUtils::LogStuff("[UVASourceComponent::Initialize()]: SoundSourceComponent initialized successfully", false);

	bInitialized = true;
}

void UVASourceComponent::UpdatePose()
{
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
	{
		return;
	}

	SoundSourceBase->SetPosition(GetPosition());
	SoundSourceBase->SetRotation(GetRotation());
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		ImageSourceModel->UpdateISPositions();
		ImageSourceModel->UpdateISRotations();
	}
}

bool UVASourceComponent::ForceUpdateSignalSourceType(TSubclassOf<UVAAbstractSignalSource> SignalSourceTypeN)
{
	if (SignalSource != nullptr)
	{
		UnbindSignalSourceEvents();
		SignalSource->Rename(*MakeUniqueObjectName(this, UVAAbstractSignalSource::StaticClass(), "SignalSource_EXPIRED").ToString());
		SignalSource->MarkPendingKill();
		SignalSource = nullptr;
	}

	if (SignalSourceTypeN == UVAAudiofileSignalSource::StaticClass())
	{
		SignalSource = NewObject<UVAAbstractSignalSource>(this, SignalSourceTypeN);
	}
	else if (SignalSourceTypeN == UVAJetEngineSignalSource::StaticClass())
	{
		SignalSource = NewObject<UVAAbstractSignalSource>(this, SignalSourceTypeN);
	}
	else if (SignalSourceTypeN != nullptr)
	{
		FVAUtils::OpenMessageBox("[UVASourceComponent::PostEditChangeProperty()]: Signal source type is not supported", true);
		return false;
	}

	SignalSourceType = SignalSourceTypeN;
	BindSignalSourceEvents();
	return true;
}

bool UVASourceComponent::SetSignalSourceID(const std::string& SignalSourceID)
{
	if (!SoundSourceBase.IsValid())
	{
		FVAUtils::OpenMessageBox(FString("[UVASourceComponent::SetSignalSourceID]: VA sound source not initialized"), true);
		return false;
	}
	if (!SoundSourceBase->SetSignalSource(SignalSourceID))
	{
		return false;
	}
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		return ImageSourceModel->UpdateISSignalSource();
	}
	return true;
}

void UVASourceComponent::OnSignalSourceIDChanged(const std::string& SignalSourceID)
{
	SetSignalSourceID(SignalSourceID);
}

void UVASourceComponent::BindSignalSourceEvents()
{
	UVAAudiofileSignalSource* AudioSignalSource = Cast<UVAAudiofileSignalSource>(SignalSource);
	if (AudioSignalSource && !AudioSignalSource->OnAudiofileChanged().IsBoundToObject(this))
	{
		SignalSourceChangedDelegate = AudioSignalSource->OnAudiofileChanged().AddUObject(this, &UVASourceComponent::OnSignalSourceIDChanged);
	}
}

void UVASourceComponent::UnbindSignalSourceEvents()
{
	if (SignalSourceChangedDelegate.IsValid())
	{
		UVAAudiofileSignalSource* AudioSignalSource = Cast<UVAAudiofileSignalSource>(SignalSource);
		if (AudioSignalSource)
		{
			AudioSignalSource->OnAudiofileChanged().Remove(SignalSourceChangedDelegate);
		}
		SignalSourceChangedDelegate.Reset();
	}
}

bool UVASourceComponent::ShouldSendCommand() const
{
	return (bInitialized && FVAPlugin::GetUseVA() && FVAPlugin::GetIsMaster());
}


// ****************************************************************** // 
// ******* Signal Source ******************************************** //
// ****************************************************************** //


bool UVASourceComponent::SetSignalSourceType(TSubclassOf<UVAAbstractSignalSource> SignalSourceTypeN)
{
	if(SignalSourceType == SignalSourceTypeN)
		return true;

	if (!ForceUpdateSignalSourceType(SignalSourceTypeN))
		return false;

	if (SignalSource)
	{
		SignalSource->Initialize();
		return SetSignalSourceID(SignalSource->GetID());
	}
	return true;
}

TSubclassOf<UVAAbstractSignalSource> UVASourceComponent::GetSignalSourceType() const
{
	return SignalSourceType;
}

UVAAbstractSignalSource* UVASourceComponent::GetSignalSource() const
{
	return SignalSource;
}


// ****************************************************************** // 
// ******* Sound Source Settings ************************************ //
// ****************************************************************** //

bool UVASourceComponent::MuteSound(const bool bMute)
{
	if (!ShouldSendCommand())
	{
		return false;
	}
	if (!SoundSourceBase->MuteSound(bMute))
	{
		return false;
	}
	if (bHandleReflections && ImageSourceModel.IsValid())
	{
		return ImageSourceModel->MuteIS(bMute);
	}
	return true;
}


bool UVASourceComponent::SetSoundPower(const float Power)
{
	if (!ShouldSendCommand())
	{
		return false;
	}
	if (!SoundSourceBase->SetPower(Power))
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

float UVASourceComponent::GetSoundPower() const
{
	return SoundPower;
}

// ****************************************************************** // 
// ******* Image Sources / Reflections ****************************** //
// ****************************************************************** //

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
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
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

bool UVASourceComponent::SetUsePoseOffset(const bool bNewUsePoseOffset)
{
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
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

bool UVASourceComponent::SetOffsetPosition(const FVector Pos)
{
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
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

bool UVASourceComponent::SetOffsetRotation(const FRotator Rot)
{
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
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

bool UVASourceComponent::SetDirectivityByMapping(const FString Phoneme)
{
	if (!ShouldSendCommand())
	{
		return false;
	}

	DirectivitySetting = EDirectivitySetting::Phoneme;
	DirectivityByMapping = Phoneme;

	return SoundSourceBase->SetDirectivity(CurrentReceiverActor->GetDirectivityByMapping(Phoneme));
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
		 return SoundSourceBase->RemoveDirectivity();
	}
	
	return SoundSourceBase->SetDirectivity(CurrentReceiverActor->GetDirectivityByFileName(FileName));
}

FString UVASourceComponent::GetDirectivityFileName() const
{
	if (SoundSourceBase.IsValid())
	{
		return SoundSourceBase->GetDirectivityFilename();
	}

	return "";
}



// ****************************************************************** // 
// ******* Graphical Representation ********************************* //
// ****************************************************************** //

bool UVASourceComponent::SetVisibility(const bool bVis)
{
	if (!FVAPlugin::GetUseVA() || !SoundSourceBase.IsValid())
	{
		return false;
	}

	SoundSourceBase->SetVisibility(bVis);
	return true;
}

bool UVASourceComponent::GetVisibility() const
{
	return SoundSourceBase->GetVisibility();
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


// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR

void UVASourceComponent::PreEditChange(FProperty* PropertyWhatWillChange)
{
	Super::PreEditChange(PropertyWhatWillChange);

	// TODO:
	// This is true if user directly changes the signal source component to "None"
	// However, it is also true if user changes a sub-property of SignalSource
	if (PropertyWhatWillChange->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, SignalSource))
	{
		UnbindSignalSourceEvents(); //Unbind works for now, since we bind in BeginPlay() again.
		//SignalSourceType = nullptr;
	}

}

void UVASourceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, SignalSourceType))
	{
		ForceUpdateSignalSourceType(SignalSourceType);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}


bool UVASourceComponent::CanEditChange(const FProperty* InProperty) const
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
