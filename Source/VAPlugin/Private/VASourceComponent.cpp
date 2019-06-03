// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerActor = GetOwner();
	started = false;
	firstTick = true;

	if (FVAPluginModule::isConnected()) {
		sendSoundData();
	}
	else {
		FVAPluginModule::enqueueSound(this);
	}


	face_bone_name = "CC_Base_FacialBone";

	root_component = CreateDefaultSubobject<USphereComponent>(TEXT("RootCmp"));

	skeletal_mesh_component = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshCmp"));
	skeletal_mesh_component->SetupAttachment(root_component);
	// USphereComponent* root_component;skeletal_mesh_component->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(idle_animation, default_slot_name, 0.0f, 0.0f, 1.0f, 1000);

}


bool UVASourceComponent::sendSoundData()
{
	FVector pos;
	FRotator rot;
	FTransform trans;

	switch (vMovement) {
		case EMovement::MoveWithObject :
		case EMovement::ObjectSpawnPoint :
			trans = ownerActor->GetTransform();
			pos = trans.GetLocation();
			rot = trans.GetRotation().Rotator();
			break;

		case EMovement::OwnPosition :
			pos = vPos;
			rot = vRot;
			break;
		default :
			break;
	}

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

	// soundID = FVAPluginModule::initializeSound(vSoundName, pos, rot, vGainOffset, vLoop, vDelay, vActionP);
	soundID = FVAPluginModule::initializeSoundWithReflections(vSoundName, pos, rot, vGainFactor * vGainFactor, vLoop, vDelay, IVAInterface::VA_PLAYBACK_ACTION_STOP);

	return true;
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

	else {
		// update Pos
		if (vMovement == EMovement::MoveWithObject) {

			FTransform trans = ownerActor->GetTransform();

			FVector pos = trans.GetLocation();
			FRotator rot = trans.GetRotation().Rotator();

			pos = pos + vOffset;
			
			FVAPluginModule::updateSourcePos(soundID, pos, rot);
		}
		else if (vMovement == EMovement::Human) {

			if (!skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
				return;
			}

			FVector pos = skeletal_mesh_component->GetSocketLocation(face_bone_name);

			FRotator rot = skeletal_mesh_component->GetSocketRotation(face_bone_name);
			
			// FVector current_forward = eye_rot.RotateVector(eye_forward);
			// 
			// FVector new_forward = position - eye_pos;
			// 
			// FQuat missing_rot = FQuat::FindBetweenVectors(current_forward, new_forward);
			// 
			// FQuat new_quat = missing_rot * FQuat(currentRot);
			// return new_quat;

			FVAPluginModule::updateSourcePos(soundID, pos, rot);

		}
	}
}

