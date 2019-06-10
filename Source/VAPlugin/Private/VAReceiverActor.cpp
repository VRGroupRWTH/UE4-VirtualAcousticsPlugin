// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "VADefines.h"

#include "DisplayClusterPawn.h"
#include "DisplayClusterSceneComponent.h"

#include "IDisplayCluster.h"
#include "IDisplayClusterClusterManager.h"

#include "VirtualRealityPawn.h"


#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVAReceiverActor::AVAReceiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AVAReceiverActor::EndPlay() {

	VAUtils::logStuff("In End Play!!!");
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
#else
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
        updateRealWorldPosition();
	}
}


bool AVAReceiverActor::updateVirtualWorldPosition()
{
	controller->GetPlayerViewPoint(tmpPosF, tmpRotF);

	FVAPluginModule::updateReceiverPos(tmpPosF, tmpRotF);

	timeSinceUpdate = 0.0f;
	return false;
}

bool AVAReceiverActor::updateRealWorldPosition()
{
    //include Controller.h
	// APawn* pawn = UGameplayStatics::GetPlayerCharacter(getWorld(), 0);
	// UDisplayClusterSceneComponent* rightComp;
    // TVector<Class> = pawn->GetComponentByClass(CLASS?!)
    // for(p : TVector)
    // {
    //    if(p->GetName() == "shutter_glasses")
    //    {
    //        rightComp = *p;
    //    }
    // }

	// https://devhub.vr.rwth-aachen.de/VR-Group/widgetinteraction/blob/develop/Source/WidgetInteraction/Private/WidgetInteraction.cpp
	// https://devhub.vr.rwth-aachen.de/VR-Group/unreallaunchscripts/blob/master/.misc/configurations/ndisplay/aixcave_5_sides_421.cfg
     
    //pos = rightComp->GetRealtiveLocation();
    //rot = rightComp->GetRelativeRotation().Rotator();

	if (!FVAPluginModule::isMasterAndUsed()) {
		return false;
	}
	

	auto world = GetWorld();
	if  (world == nullptr) {
		return false;
	}

	auto player_controller = world->GetFirstPlayerController();
	if  (player_controller == nullptr) {
		return false;
}

	auto vr_pawn = dynamic_cast<AVirtualRealityPawn*>(player_controller->AcknowledgedPawn);
	if  (vr_pawn == nullptr) {
		return false;
	}

    FVector pos;
    FRotator rot;

	FString name1 = "shutter_glasses";
	FString name2 = "cave_origin";

	FVector  shutter; 
	FRotator shutterRot;
	FVector  origin;
	FRotator originRot;

	UClass* component_class = UDisplayClusterSceneComponent::StaticClass();

	auto parent_vec = vr_pawn->GetComponentsByClass(component_class);
	bool suc1 = false;
	bool suc2 = false;

	for (auto parent : parent_vec) {
		if (parent->GetName() == FString(name1)) {
			auto tmp = dynamic_cast<USceneComponent*>(parent);
			
			shutter = tmp->GetComponentLocation();
			shutterRot = tmp->GetComponentRotation();
			suc1 = true;

		}
		else if (parent->GetName() == FString(name2)) {

			auto tmp = dynamic_cast<USceneComponent*>(parent);

			origin = tmp->GetComponentLocation();
			originRot = tmp->GetComponentRotation();
			suc2 = true;
		}
	}

	if (!(suc1 && suc2)) {
		return false;
	}


	pos = shutter - origin;
	rot = shutterRot - originRot;

	VAUtils::logStuff(FString("RL pos: " + pos.ToString()));
	VAUtils::logStuff(FString("RL rot: " + rot.ToString()));
	

	/*
	auto world = GetWorld();
	auto player_controller = world->GetFirstPlayerController();

	if (player_controller == nullptr) {
		return false;
	}
	auto vr_pawn = dynamic_cast<AVirtualRealityPawn*>(player_controller->AcknowledgedPawn);
	if (vr_pawn == nullptr) {
		return false;
	}

	FVector posPawn, posOrigin;
	FRotator rotPawn;


	
	UClass* component_class = UDisplayClusterSceneComponent::StaticClass();

	auto parent_vec = vr_pawn->GetComponentsByClass(component_class);
	
	for (auto parent : parent_vec) {
		if (parent->GetName() == FString("shutter_glasses"))
		{
			// TODO do sth with parent? but there is no ->GetLocation or sth
		}
		if (parent->GetName() == FString("cave_origin"))
		{
			// TODO do sth with parent? but there is no ->GetLocation or sth
		}
	}

	pos = posPawn - posOrigin;
	rot = rotPawn;
    */

    return FVAPluginModule::updateReceiverRealWorldPos(pos, rot);
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



