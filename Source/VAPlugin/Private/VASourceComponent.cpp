// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"
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

	TArray<AActor*> recActors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReceiverActor::StaticClass(), recActors);
	AVAReceiverActor* tmp = nullptr;

	for (AActor* iter : recActors)
	{
		// if there is an ReceiverActor in the Szene
		tmp = dynamic_cast<AVAReceiverActor*>(iter);
		if (tmp != nullptr)
		{
			VAUtils::logStuff("[UVASourceComponent::BeginPlay()]: AVAReceiver found");
			break;
		}
	}

	// If no Rec Actor found spawn one with default parameters
	if (tmp == nullptr)
	{
		VAUtils::logStuff("[UVASourceComponent::BeginPlay()]: No AVAReceiver found! Spawning one with default values");
		tmp = this->GetWorld()->SpawnActor<AVAReceiverActor>(AVAReceiverActor::StaticClass());
	}

	// If the receiver Actor is initialized but this sound Component not, this Component is spawned at runtime and has to be initialized
	if (tmp->isInitialized() && !bInitialized)
	{
		Initialize();
	}
}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FVAPluginModule::getUseVA())
	{
		return;
	}

	if (!bInitialized)
	{
		VAUtils::openMessageBox("A Sound source is not initialized");
	}

	if (bFirstTick && FVAPluginModule::getIsMaster())
	{
		timeSinceUpdate = 1.0f;
		if (startingPlayAction == Play)
		{
			SoundSource->playSoundFromSecond(startingTime);
		}
	}
	bFirstTick = false;


	timeSinceUpdate += DeltaTime;

	if ((movementSetting == AttatchToBone || movementSetting == MoveWithObject) &&
		timeSinceUpdate > (1.0f / 30.0f))
	{
		SoundSource->setPos(GetPosition());
		SoundSource->setRot(GetRotation());

		timeSinceUpdate = 0.0f;
	}
}


void UVASourceComponent::Initialize()
{
	if (!FVAPluginModule::getUseVA() || bInitialized)
	{
		return;
	}

	OwnerActor = GetOwner();
	bFirstTick = true;


	SkeletalMeshComponent = dynamic_cast<USkeletalMeshComponent*>(
		OwnerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (movementSetting == AttatchToBone)
	{
		if (SkeletalMeshComponent != nullptr
			&& SkeletalMeshComponent->DoesSocketExist(FName(*boneName)))
		{
			VAUtils::logStuff("Bone detected.");
		}
		else
		{
			VAUtils::openMessageBox("Error: Could not find bone, using MoveWithObject instead.");
			movementSetting = MoveWithObject;
		}
	}


	spawnPosition = OwnerActor->GetTransform().GetLocation();
	spawnRotation = OwnerActor->GetTransform().GetRotation().Rotator();


	SoundSource = new VASoundSource(this);

	if (FVAPluginModule::getIsMaster())
	{
		switch (directivitySetting)
		{
		case DefaultHRIR:
			SoundSource->setDirectivity(VADirectivityManager::getDefaultDirectivity());
			break;

		case manualFile:
			SoundSource->setDirectivity(
				AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByFileName(directivityByFileName));
			break;

		case phoneme:
			SoundSource->setDirectivity(
				AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByMapping(directivityByMapping));
			break;

		default:
			break;
		}
	}

	VAUtils::logStuff("SoundSourceComponent initialized successfully");

	bInitialized = true;
}

bool UVASourceComponent::shouldSendCommand()
{
	return (bInitialized && FVAPluginModule::getUseVA() && FVAPluginModule::getIsMaster());
}


// ****************************************************************** // 
// ******* Interface Sound ****************************************** //
// ****************************************************************** //

bool UVASourceComponent::PlaySound()
{
	if (!shouldSendCommand())
	{
		return false;
	}

	SoundSource->playSound();

	return true;
}

bool UVASourceComponent::PlaySoundFromSecond(float time)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	SoundSource->playSoundFromSecond(time);

	return true;
}

bool UVASourceComponent::StopSound()
{
	if (!shouldSendCommand())
	{
		return false;
	}

	SoundSource->stopSound();

	return true;
}

bool UVASourceComponent::PauseSound()
{
	if (!shouldSendCommand())
	{
		return false;
	}

	SoundSource->pauseSound();

	return true;
}

EPlayAction UVASourceComponent::GetPlayState()
{
	if (!shouldSendCommand())
	{
		return Stop;
	}

	return VAUtils::VAActionToEPlayAction(SoundSource->getPlayState());
}

void UVASourceComponent::MuteSound(bool mute_)
{
	if (!shouldSendCommand())
	{
		return;
	}

	if (muted == mute_)
	{
		return;
	}

	SoundSource->muteSound(muted);
}


// ****************************************************************** // 
// ******* Sound Pose *********************************************** //
// ****************************************************************** //

FVector UVASourceComponent::GetPosition()
{
	FVector pos;
	switch (movementSetting)
	{
	case MoveWithObject:
		pos = OwnerActor->GetTransform().GetLocation();
		break;

	case ObjectSpawnPoint:
		pos = spawnPosition;
		break;

	case AttatchToBone:
		pos = SkeletalMeshComponent->GetSocketLocation(FName(*boneName));
		break;

	default:
		pos = FVector::ZeroVector;
		VAUtils::logStuff(FString("[UVASourceComponent::getPosition()]: default"));
		break;
	}

	if (usePoseOffset)
	{
		pos = pos + offsetPosition;
	}

	return pos;
}

FRotator UVASourceComponent::GetRotation()
{
	FRotator rot;
	switch (movementSetting)
	{
	case MoveWithObject:
		rot = OwnerActor->GetTransform().GetRotation().Rotator();
		break;

	case ObjectSpawnPoint:
		rot = spawnRotation;
		break;

	case AttatchToBone:
		rot = SkeletalMeshComponent->GetSocketRotation(FName(*boneName));
		break;

	default:
		VAUtils::logStuff(FString("[UVASourceComponent::getRotation()]: default"));
		rot = FRotator::ZeroRotator;
		break;
	}

	if (usePoseOffset)
	{
		FQuat quat(rot);
		quat = quat * FQuat(offsetRotation);
		rot = FRotator(quat);
	}

	return rot;
}


// ****************************************************************** // 
// ******* Sound Settings ******************************************* //
// ****************************************************************** //

bool UVASourceComponent::SetSoundSourceVisibility(bool vis)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	SoundSource->setVisibility(vis);

	return true;
}


bool UVASourceComponent::LoadSoundFile(FString soundFile_)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	return SoundSource->loadNewSound(soundFile);
}

bool UVASourceComponent::SetSoundFile(FString soundFile_)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	// If already playing back that sound File
	if (soundFile == soundFile_)
	{
		SoundSource->stopSound();
		return true;
	}

	soundFile = soundFile_;

	return SoundSource->setNewSound(soundFile);
}


bool UVASourceComponent::setSoundPower(float power)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	// If already playing back that sound File
	if (soundPower == power)
	{
		return true;
	}

	soundPower = power;

	SoundSource->setPower(power);

	return true;
}


bool UVASourceComponent::setLoop(bool loop_)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	// If this setting is already set properly
	if (loop == loop_)
	{
		return true;
	}

	loop = loop_;


	SoundSource->setLoop(loop);

	return true;
}


bool UVASourceComponent::setUsePoseOffset(bool usePoseOffset_)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (usePoseOffset == usePoseOffset_)
	{
		return true;
	}

	usePoseOffset = usePoseOffset_;

	SoundSource->setPos(GetPosition());
	SoundSource->setRot(GetRotation());

	return true;
}

bool UVASourceComponent::setOffsetPosition(FVector pos_)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (offsetPosition == pos_)
	{
		return true;
	}

	offsetPosition = pos_;

	SoundSource->setPos(GetPosition());

	return true;
}

bool UVASourceComponent::setOffsetRotation(FRotator rot_)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (offsetRotation == rot_)
	{
		return true;
	}

	offsetRotation = rot_;

	SoundSource->setRot(GetRotation());

	return true;
}

bool UVASourceComponent::setBoneName(FString boneName_)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (boneName == boneName_)
	{
		return true;
	}

	boneName = boneName_;


	// Check if the bone exists
	if (SkeletalMeshComponent != nullptr
		&& SkeletalMeshComponent->DoesSocketExist(FName(*boneName_)))
	{
		boneName = boneName_;
		movementSetting = AttatchToBone;
		return true;
	}
	VAUtils::openMessageBox(
		"[UVASourceComponent::setBoneName(FString)]: Could not find new bone, using old settings instead.");
	return false;
}


bool UVASourceComponent::setMovementSetting(EMovement movementSetting_)
{
	if (!FVAPluginModule::getUseVA())
	{
		return false;
	}

	// If this setting is already set properly
	if (movementSetting == movementSetting_)
	{
		return true;
	}

	movementSetting = movementSetting_;


	SoundSource->setPos(GetPosition());
	SoundSource->setRot(GetRotation());

	return true;
}


// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //

bool UVASourceComponent::SetDirectivityByMapping(FString phoneme)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	directivitySetting = EDir::phoneme;
	directivityByMapping = phoneme;

	SoundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByMapping(phoneme));

	return true;
}

bool UVASourceComponent::SetDirectivityByFileName(FString fileName)
{
	if (!shouldSendCommand())
	{
		return false;
	}

	directivitySetting = manualFile;
	directivityByFileName = fileName;

	SoundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByFileName(fileName));

	return true;
}


// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

bool UVASourceComponent::getVisibility()
{
	return FVAPluginModule::isInDebugMode();
}

bool UVASourceComponent::getHandleReflections()
{
	return handleReflections;
}

float UVASourceComponent::getSoundTimeOffset()
{
	return startingTime;
}

FString UVASourceComponent::getBoneName()
{
	if (movementSetting != AttatchToBone)
	{
		VAUtils::logStuff("[UVASourceComponent::getBoneName()]: Movement is not set to AttatchToBone..");
	}
	return boneName;
}

FString UVASourceComponent::GetDirectivityFileName()
{
	if (SoundSource != nullptr)
	{
		return SoundSource->getDirectivity()->getFileName();
	}

	return "";
}

bool UVASourceComponent::getLoop()
{
	return loop;
}

FString UVASourceComponent::GetSoundFile()
{
	return soundFile;
}

float UVASourceComponent::getSoundPower()
{
	return soundPower;
}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR
bool UVASourceComponent::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, boneName))
	{
		return movementSetting == AttatchToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, directivityByFileName))
	{
		return directivitySetting == manualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, directivityByMapping))
	{
		return directivitySetting == phoneme;
	}


	return ParentVal;
}
#endif
