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


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (firstTick) {
		firstTick = false;
		if (vAction == EPlayAction::Play) {
			FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
		}
	}


	if (!started) {
		timer += DeltaTime;
		if (timer > vDelay) {
			playSound();
			started = true;
		}
	}
	else {
		// update Pos
		if (vMovement == EMovement::MoveWithObject) {


			FTransform trans = ownerActor->GetTransform();

			FVector pos = trans.GetLocation();
			FRotator rot = trans.GetRotation().Rotator();

			pos = pos + vOffset;
			
			FVAPluginModule::updateSourcePos(soundID, pos, rot);
		}
	}
}

