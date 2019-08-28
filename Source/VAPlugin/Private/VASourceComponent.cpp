// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// root_component = CreateDefaultSubobject<USphereComponent>(TEXT("RootCmp"));
	// 
	// skeletal_mesh_component = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshCmp"));
	// skeletal_mesh_component->SetupAttachment(root_component);
}


// Called when the game starts
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();

	// USphereComponent* root_component;skeletal_mesh_component->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(idle_animation, default_slot_name, 0.0f, 0.0f, 1.0f, 1000);

}


bool UVASourceComponent::sendSoundData()
{
	if (alreadySent) {
		VAUtils::openMessageBox("Send Sound data for multiple times");
		return false;
	}

	alreadySent = true;

	FVector pos;
	FRotator rot;

	pos = getPosition();
	rot = getRotation();

	/*
	int vActionP;
	switch (vAction)
	{
		case EPlayAction::Play :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_PLAY;
			break;

		case EPlayAction::Pause :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_PAUSE;
			break;

		case EPlayAction::Stop :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
			break;

		default: 
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
	}
	*/

	VAUtils::logStuff(FString("pos for ini: " + pos.ToString()));

	// soundID = FVAPluginModule::initializeSound(vSoundName, pos, rot, vGainOffset, vLoop, vDelay, vActionP);
	soundID = FVAPluginModule::initializeSoundWithReflections(vSoundName, pos, rot, vGainFactor * vGainFactor, vLoop, vDelay, IVAInterface::VA_PLAYBACK_ACTION_STOP);

	return true;
}

void UVASourceComponent::initialize()
{
	ownerActor = GetOwner();
	started = false;
	firstTick = true;
	alreadySent = false;

	skeletal_mesh_component = dynamic_cast<USkeletalMeshComponent*> (ownerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	// face_bone_name = "CC_Base_FacialBone";
	face_bone_name = "CC_Base_L_Eye";

	if (skeletal_mesh_component != nullptr && skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
		vMovement = EMovement::Human;
		VAUtils::openMessageBox("Human detected");
	}


	if (FVAPluginModule::isConnected()) {
		sendSoundData();
	}
	else {
		FVAPluginModule::enqueueSound(this);
	}

}

void UVASourceComponent::playSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
}

void UVASourceComponent::stopSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::pauseSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::updatePosition(FVector vec, FRotator rot)
{
	FVAPluginModule::updateSourcePos(soundID, vec, rot);
}

void UVASourceComponent::playSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
}

void UVASourceComponent::stopSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::pauseSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::updatePositionWithReflections(FVector vec, FRotator rot)
{
	FVAPluginModule::updateSourcePosWithReflections(soundID, vec, rot);
}

FVector UVASourceComponent::getPosition()
{
	switch (vMovement) {
		case EMovement::MoveWithObject:
			return ownerActor->GetTransform().GetLocation() + vOffset;
			break;

		case EMovement::ObjectSpawnPoint:
			return ownerActor->GetTransform().GetLocation() + vOffset;
			break;

		case EMovement::OwnPosition:
			return vPos + vOffset;
			break;

		case EMovement::Human:
			if (!skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
				VAUtils::logStuff(FString("Could not find face_bone in getPosition"));
				break;
			}
			return skeletal_mesh_component->GetSocketLocation(face_bone_name) + vOffset;
			break;
		
		default:
			VAUtils::logStuff(FString("default: in getPosition"));
			break;
	}

	return FVector::ZeroVector;
}

FRotator UVASourceComponent::getRotation()
{
	switch (vMovement) {
	case EMovement::MoveWithObject:
		return ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::ObjectSpawnPoint:
		return ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::OwnPosition:
		return vRot;
		break;

	case EMovement::Human:
		if (!skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
			VAUtils::logStuff(FString("Could not find face_bone in getRotation"));
			break;
		}
		return skeletal_mesh_component->GetSocketRotation(face_bone_name);
		break;

	default:
		VAUtils::logStuff(FString("default: in getRotation"));
		break;
	}
	
	return FRotator::ZeroRotator;
}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (firstTick) {
		firstTick = false;
		if (vAction == EPlayAction::Play) {
			FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
			started = true;
		}
	}

	
	// if (!started) {
	// 	timer += DeltaTime;
	// 	if (timer > vDelay) {
	// 		playSoundWithReflections();
	// 		started = true;
	// 	}
	// }
	

	// update if object is / could be moving
	if(vMovement == EMovement::Human || vMovement == EMovement::MoveWithObject) {
		FVector pos = getPosition();
		FRotator rot = getRotation();
		FVAPluginModule::updateSourcePos(soundID, pos, rot);
	}
	
}

