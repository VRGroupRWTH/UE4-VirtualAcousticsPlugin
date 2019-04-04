// Fill out your copyright notice in the Description page of Project Settings.

#include "VAReceiverActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "VADefines.h"

// Sets default values
AVAReceiverActor::AVAReceiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVAReceiverActor::BeginPlay()
{
	Super::BeginPlay();

	tmpPos = new VAVec3();
	tmpQuat = new VAQuat();

	timeSinceUpdate = 0.0f;

	controller = GetWorld()->GetFirstPlayerController();
	
	FVAPluginModule::initializeServer();
	FVAPluginModule::initializeReceiver(this);
	FVAPluginModule::processSoundQueue();



	
}

// Called every frame
void AVAReceiverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeSinceUpdate += DeltaTime;

	if (timeSinceUpdate > (1.0f / 91)) {
		updateVirtualWorldPosition();
	}
}


bool AVAReceiverActor::updateVirtualWorldPosition()
{
	controller->GetPlayerViewPoint(tmpPosF, tmpRotF);

	FVAPluginModule::updateReceiverPos(tmpPosF, tmpRotF);

	timeSinceUpdate = 0.0f;
	return false;
}

std::string AVAReceiverActor::getDirectivity()
{
	std::string sDirect;

	switch (vDirectivity)
	{
		case DefaultHRIR:
			sDirect = "$(DefaultHRIR)";
			break;
		default:
			sDirect = "$(DefaultHRIR)";
			break;
	}
	
	return sDirect;
}

float AVAReceiverActor::getScale()
{
	return vScale;
}

FString AVAReceiverActor::getIPAdress()
{
	if (vIPAdress.ip1 == 127 && vIPAdress.ip2 == 0 && vIPAdress.ip3 == 0 && vIPAdress.ip4 == 1) {
		return "localhost";
	}
	else {
		VAUtils::openMessageBox("could not translate IP Adress, using localhost", true); // TODO: implement other IP Adresses
	}

	return "localhost";
}

float AVAReceiverActor::getGainFactor()
{
	return vGainFactor;
}


