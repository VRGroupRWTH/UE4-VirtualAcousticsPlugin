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

	int action;
	if (vDelay == 0.0f) {
		action = IVAInterface::VA_PLAYBACK_ACTION_PLAY;
		started = true;
	}
	else {
		action = IVAInterface::VA_PLAYBACK_ACTION_PAUSE;
	}
	
	soundID = FVAPluginModule::initializeSound(vSoundName, pos, rot, vGainOffset, vLoop, action);

	return true;
}



// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!started)
	{
		timer += DeltaTime;
		if (timer > vDelay) {
			FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
			started = true;
		}
	}
	else
	{
		// update Pos
		if (vMovement == EMovement::MoveWithObject)
		{
			// TODO update Source Pos
		}

	}
}

