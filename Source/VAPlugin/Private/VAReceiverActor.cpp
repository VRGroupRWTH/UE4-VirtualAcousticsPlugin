// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "VADefines.h"


#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

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
	
	
    FString adresse;
#if PLATFORM_WINDOWS
    adresse = "localhost";
#elseif
    adresse = "10.0.1.240";
#endif

	// Connect to VA Server
	FVAPluginModule::initializeServer(adresse, vPort);
	
	// Initialize Receiver Actor
	FVAPluginModule::initializeReceiver(this);
	
	// Initialize Walls for Sound Reflection
	TArray<AActor*> wallsA;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReflectionWall::StaticClass(), wallsA);
	TArray<AVAReflectionWall*> walls;
	for (AActor* actor : wallsA) {
		walls.Add((AVAReflectionWall*)actor);
	}
	FVAPluginModule::initializeWalls(walls);

	// Initialize Sounds that could not have been processed earlier because of the missing connection to the VA Server
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
	if (vAdress == EAdress::localhost) {
		return "localhost";
	}
	else if (vAdress == EAdress::Cave) {
		return "10.0.1.240";
	}

	VAUtils::openMessageBox("could not evaluate IP Adress. Returning localhost (AVAReceiverActor::getIPAdress())", true);

	return "localhost";
}

float AVAReceiverActor::getGainFactor()
{
	return vGainFactor;
}



