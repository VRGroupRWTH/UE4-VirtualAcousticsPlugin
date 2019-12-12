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
UVASourceComponent::UVASourceComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVASourceComponent::BeginPlay() {
	Super::BeginPlay();
	initialized = false;
	firstTick = true;
}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FVAPluginModule::getUseVA()) {
		return;
	}

	if (!initialized) {
		VAUtils::openMessageBox("A Sound source is not initialized");
	}

	if (firstTick && isMaster) {
		timeSinceUpdate = 1.0f;
		if (vAction == EPlayAction::Play) {
			soundSource->playSound();
		}
	}
	firstTick = false;


	timeSinceUpdate += DeltaTime;

	if ((vMovement == EMovement::AttatchToBone || vMovement == EMovement::MoveWithObject) && 
		timeSinceUpdate > (1.0f / 30.0f)) {

		// Checks himself if only updating graphical (in every node, not just master) or send information
		
		soundSource->setPos(getPosition());
		soundSource->setRot(getRotation());
		
		// soundSource->setPos(FVector(0, 0, 0));
		// soundSource->setRot(FRotator(0, 0, 0));

		timeSinceUpdate = 0.0f;
	}

}


void UVASourceComponent::initialize()
{
	ownerActor = GetOwner();
	firstTick = true;

	isMaster = FVAPluginModule::getIsMaster();

	skeletal_mesh_component = dynamic_cast<USkeletalMeshComponent*> (ownerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (vMovement == EMovement::AttatchToBone) {
		if (skeletal_mesh_component != nullptr 
			&& skeletal_mesh_component->DoesSocketExist(vBoneName)) {
			VAUtils::logStuff("Bone detected.");
		}
		else {
			VAUtils::openMessageBox("Error: Could not find bone, using MoveWithObject instead.");
			vMovement = EMovement::MoveWithObject;
		}
	}

	if (vMovement == EMovement::ObjectSpawnPoint) {
		spawnPosition = ownerActor->GetTransform().GetLocation();
		spawnRotation = ownerActor->GetTransform().GetRotation().Rotator();
	}

	soundSource = new VASoundSource(this);

	if (FVAPluginModule::getIsMaster()) {
		switch (vDirectivity) {
			case EDir::DefaultHRIR : 
				soundSource->setDirectivity(VADirectivityManager::getDefaultSourceDirectivity());
				break;

			case EDir::manualFile :
				// VADirectivity* tmp = AVAReceiverActor::getCurrentReceiverActor()->getDirectvityByPhoneme(vDirectivityByPhoneme);
				// soundSource->setDirectivity(new VADirectivity(vDirectivityByFileName));
				break;

			case EDir::phoneme :
				// VADirectivity* tmp = AVAReceiverActor::getCurrentReceiverActor()->getDirectvityByPhoneme(vDirectivityByPhoneme);
				// 
				// if (tmp != nullptr) {
				// 	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectvityByPhoneme(vDirectivityByPhoneme));
				// }
				// else {
				// 	
				// }
				break;
			default:
				break;
		}
	}
	
	VAUtils::logStuff("SoundSourceComponent initialized successfully");

	initialized = true;

}

// ****************************************************************** // 
// ******* Interface Sound ****************************************** //
// ****************************************************************** //

void UVASourceComponent::playSound()
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }

	soundSource->playSound();
}

void UVASourceComponent::playSoundFromSecond(float time)
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }

	soundSource->playSoundFromSecond(time);
}

void UVASourceComponent::stopSound()
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }

	soundSource->stopSound();
}

void UVASourceComponent::pauseSound()
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }

	soundSource->pauseSound();
}


// ****************************************************************** // 
// ******* Sound Pose *********************************************** //
// ****************************************************************** //

FVector UVASourceComponent::getPosition()
{
	FVector pos; 
	switch (vMovement) {
		case EMovement::MoveWithObject:
			pos = ownerActor->GetTransform().GetLocation();
			break;

		case EMovement::ObjectSpawnPoint:
			pos = spawnPosition;
			break;

		case EMovement::AttatchToBone:
			pos = skeletal_mesh_component->GetSocketLocation(vBoneName);
			break;
		
		default:
			pos = FVector::ZeroVector;
			VAUtils::logStuff(FString("[UVASourceComponent::getPosition()]: default"));
			break;
	}

	if (vUseOffset) {
		pos = pos + vOffsetPosition;
	}

	return pos;
}

FRotator UVASourceComponent::getRotation()
{
	FRotator rot;
	switch (vMovement) {
	case EMovement::MoveWithObject:
		rot = ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::ObjectSpawnPoint:
		rot = spawnRotation;
		break;

	case EMovement::AttatchToBone:
		rot = skeletal_mesh_component->GetSocketRotation(vBoneName);
		break;

	default:
		VAUtils::logStuff(FString("[UVASourceComponent::getRotation()]: default"));
		rot = FRotator::ZeroRotator;
		break;
	}

	if (vUseOffset) {
		FQuat quat(rot);
		quat = quat * FQuat(vOffsetRotation);
		rot = FRotator(quat);
	}

	return rot;
}

void UVASourceComponent::setSoundSourcePosition(FVector pos) {
  if (!FVAPluginModule::getUseVA()) {
    return;
  }
	soundSource->setPos(pos);
}

void UVASourceComponent::setSoundSourceRotation(FRotator rot) {
  if (!FVAPluginModule::getUseVA()) {
    return;
  }
	soundSource->setRot(rot);
}


// ****************************************************************** // 
// ******* Sound Settings ******************************************* //
// ****************************************************************** //

void UVASourceComponent::setDirectivityByPhoneme(FString phoneme)
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }
	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectvityByPhoneme(phoneme));
}

void UVASourceComponent::setSoundSourceVisibility(bool vis)
{
  if (!FVAPluginModule::getUseVA()) {
    return;
  }
	soundSource->setVisibility(vis);
}



// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //




// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

bool UVASourceComponent::getVisibility()
{
	return FVAPluginModule::isInDebugMode();
}

bool UVASourceComponent::getHandleReflections() {
	return vHandleReflections;
}

bool UVASourceComponent::getLoop() {
	return vLoop;
}

float UVASourceComponent::getGainFactor() {
	return vGainFactor;
}

float UVASourceComponent::getSoundTimeOffset() {
	return vDelay;
}

FString UVASourceComponent::getFileName() {
	return vSoundName;
}


// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR
bool UVASourceComponent::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vBoneName))
	{
		return vMovement == EMovement::AttatchToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vDirectivityByFileName))
	{
		return vDirectivity == EDir::manualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vDirectivityByPhoneme))
	{
		return vDirectivity == EDir::phoneme;
	}


	return ParentVal;
}
#endif