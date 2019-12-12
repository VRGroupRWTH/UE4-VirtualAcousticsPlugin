// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "VADefines.h"

#include "DisplayClusterPawn.h"
#include "DisplayClusterSceneComponent.h"

#include "Engine.h"									 // For Events
#include "IDisplayCluster.h"						 // For Events
#include "IDisplayClusterClusterManager.h"

#include "VirtualRealityPawn.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"


AVAReceiverActor* AVAReceiverActor::currentReceiverActor;

// Sets default values
AVAReceiverActor::AVAReceiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	wallsInitialized = false;

}


// ****************************************************************** // 
// ******* Initialization Functions ********************************* //
// ****************************************************************** //

void AVAReceiverActor::BeginPlay()
{
	Super::BeginPlay();


	FVAPluginModule::setReceiverActor(this);

	currentReceiverActor = this;

	// Ask if used or not
	FVAPluginModule::askForSettings(getIPAdress(), getPort(), askForDebugMode);

	if (FVAPluginModule::getIsMaster()) {
		if (FVAPluginModule::getUseVA()) {
			runOnAllNodes("useVA = true");
		}
		else {
			runOnAllNodes("useVA = false");
			return;
		}
	}

	wallsInitialized = false;
	
	timeSinceUpdate = 0.0f;
	totalTime = 0.0f;


	// Controller for vision // 
	controller = GetWorld()->GetFirstPlayerController();
	

	// Cluster Stuff for Events //
	IDisplayClusterClusterManager* ClusterManager = IDisplayCluster::Get().GetClusterMgr();
	if (ClusterManager && !ClusterEventListenerDelegate.IsBound())
	{
		ClusterEventListenerDelegate = FOnClusterEventListener::CreateUObject(this, &AVAReceiverActor::HandleClusterEvent);
		ClusterManager->AddClusterEventListener(ClusterEventListenerDelegate);
	}

	if (!FVAPluginModule::getUseVA()) {
		return;
	}


	if (FVAPluginModule::getIsMaster()) {

		FVAPluginModule::setScale(worldScale);

		if (!FVAPluginModule::isConnected()) {
			FVAPluginModule::connectServer(getIPAdress(), getPort());
		}
		else {
			FVAPluginModule::resetServer();
		}
	
		// Initialize the dirManager
		dirManager.readConfigFile(dirMappingFileName);
	
		// Initialize Receiver Actor
		receiverID = FVAPluginModule::createNewSoundReceiver(this);
	}

	// Initialize Walls for Sound Reflection
	if (!wallsInitialized) {
		initializeWalls();
	}


	// Handle all sound Sources
	TArray<AActor*> actorsA;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), actorsA);
	
	UVASourceComponent* tmp;

	for (AActor* actor : actorsA) {
		tmp = dynamic_cast<UVASourceComponent*> (actor->GetComponentByClass(UVASourceComponent::StaticClass()));
		if (tmp != nullptr) {
			tmp->initialize();
		}
	}


	/*
	if (FVAPluginModule::getIsMaster()) {
		if (FVAPluginModule::getDebugMode()) {
			runOnAllNodes("debugMode = true");
		}
		else {
			runOnAllNodes("debugMode = false");
		}
	}
	*/
	if (FVAPluginModule::getIsMaster()) {
		if (FVAPluginModule::getDebugMode()) {
			runOnAllNodes("debugMode = true");
		}
		else {
			runOnAllNodes("debugMode = false");
		}
	}
}

void AVAReceiverActor::BeginDestroy()
{
	Super::BeginDestroy();
	
	FVAPluginModule::resetServer();

	dirManager.reset();

	IDisplayClusterClusterManager* ClusterManager = IDisplayCluster::Get().GetClusterMgr();
	if (ClusterManager && ClusterEventListenerDelegate.IsBound())
	{
		ClusterManager->RemoveClusterEventListener(ClusterEventListenerDelegate);
	}
}

void AVAReceiverActor::initializeWalls()
{
	TArray<AActor*> wallsA;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReflectionWall::StaticClass(), wallsA);
	for (AActor* actor : wallsA) {
		reflectionWalls.Add((AVAReflectionWall*)actor);
	}
	wallsInitialized = true;
}


// ****************************************************************** // 
// ******* Tick Function ******************************************** //
// ****************************************************************** //

void AVAReceiverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FVAPluginModule::getUseVA() || !FVAPluginModule::getIsMaster()) {
		return;
	}

	timeSinceUpdate += DeltaTime;
	totalTime += DeltaTime;

	if (timeSinceUpdate > (1.0f / 30.0f)) {
		updateVirtualWorldPosition();
		// FVAPluginModule::setSoundReceiverPosition(receiverID, FVector(100,0,0));
		// FVAPluginModule::setSoundReceiverRotation(receiverID, FRotator(0,0,0));
		updateRealWorldPosition();

		timeSinceUpdate = 0.0f;
	}

	/*
	if (totalTime > 2.0f) {
		if (FVAPluginModule::getDebugMode()) {
			runOnAllNodes("debugMode = false");
		}
		else {
			runOnAllNodes("debugMode = true");
		}
		totalTime = 0.0f;
	}
	*/	
}


// ****************************************************************** // 
// ******* Position updates ***************************************** //
// ****************************************************************** //

bool AVAReceiverActor::updateVirtualWorldPosition()
{
	controller->GetPlayerViewPoint(tmpPosF, tmpRotF);

	FVAPluginModule::setSoundReceiverPosition(receiverID, tmpPosF);
	FVAPluginModule::setSoundReceiverRotation(receiverID, tmpRotF);

	return false;
}

bool AVAReceiverActor::updateRealWorldPosition()
{
	if (!(FVAPluginModule::getIsMaster() && FVAPluginModule::getUseVA())) {
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

	auto head = vr_pawn->GetHeadComponent();
	auto origin = vr_pawn->GetTrackingOriginComponent();

	if (!head || !origin)
		return false;

	// calculate positions
	FVector pos = head->GetComponentLocation() - origin->GetComponentLocation();
	FRotator rot = head->GetComponentRotation() - origin->GetComponentRotation();
	FQuat quat = origin->GetComponentQuat().Inverse() * head->GetComponentQuat();

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, TEXT("RL pos: " + pos.ToString() + "  RL rot: " + rot.RotateVector(FVector(1,0,0)).ToString()+ "  RL quat: "+quat.RotateVector(FVector(1, 0, 0)).ToString()));
	//VAUtils::logStuff(FString("RL pos: " + pos.ToString() + "  RL rot: " + rot.ToString()));
	
    return FVAPluginModule::setSoundReceiverRealWorldPose(receiverID, pos, rot);
}


// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

float AVAReceiverActor::getScale()
{
	return worldScale;
}

FString AVAReceiverActor::getIPAdress()
{

	switch (adressSetting)
	{
		case EAdress::automatic :
#if PLATFORM_WINDOWS
			return FString("localhost");
#else
			return FString("10.0.1.240");
#endif
			break;
		case EAdress::Cave :
			return FString("10.0.1.240");
			break;
		case EAdress::localhost :
			return FString("localhost");
			break;
		case EAdress::manual :
			return serverIPAdress;
			break;
		default :
			break;
			
	}

	VAUtils::logStuff("Error in AVAReceiverActor::getIPAdress()", true);

	return FString("localhost");
}

int AVAReceiverActor::getPort()
{
	switch (adressSetting)
	{
		case EAdress::automatic : 
		case EAdress::Cave :
		case EAdress::localhost :
			return 12340;
			break;
		case EAdress::manual:
			return serverPort;
			break;
		default:
			break;
	}

	VAUtils::logStuff("Error in AVAReceiverActor::getPort()", true);

	return 12340;
}

TArray<AVAReflectionWall*> AVAReceiverActor::getReflectionWalls()
{
	if (!wallsInitialized) {
		initializeWalls();
	}
	return reflectionWalls;
}


// ****************************************************************** // 
// ******* Directivity Handling ************************************* //
// ****************************************************************** //

VADirectivity* AVAReceiverActor::getDirectvityByPhoneme(FString phoneme) 
{
	return dirManager.getDirectivityByPhoneme(phoneme);
}




// ****************************************************************** // 
// ******* Cluster Stuff ******************************************** // 
// ****************************************************************** //

void AVAReceiverActor::runOnAllNodes(FString command)
{
	IDisplayClusterClusterManager* const Manager = IDisplayCluster::Get().GetClusterMgr();
	if (Manager)
	{
		if (Manager->IsStandalone()) {
			//in standalone (e.g., desktop editor play) cluster events are not executed....
			handleClusterCommand(command);
			VAUtils::logStuff("Cluster Command " + command + " ran localy");
		}
		else {
			// else create a cluster event to react to
			FDisplayClusterClusterEvent cluster_event;
			cluster_event.Name = command;
			Manager->EmitClusterEvent(cluster_event, true);
			VAUtils::logStuff("Cluster Command " + command + " sent");
		}
	}

}

AVAReceiverActor* AVAReceiverActor::getCurrentReceiverActor()
{
	return currentReceiverActor;
}

void AVAReceiverActor::HandleClusterEvent(const FDisplayClusterClusterEvent & Event)
{
	handleClusterCommand(Event.Name);

}

void AVAReceiverActor::handleClusterCommand(FString command)
{
	VAUtils::logStuff("Cluster Command " + command + " received");
	if (command == "useVA = true") {
		FVAPluginModule::setUseVA(true);
	}
	else if (command == "useVA = false") {
		FVAPluginModule::setUseVA(false);
	}
	else if (command == "debugMode = true") {
		FVAPluginModule::setDebugMode(true);
	}
	else if (command == "debugMode = false") {
		FVAPluginModule::setDebugMode(false);
	}
	else {
		VAUtils::logStuff("Cluster Command " + command + " could not have been found.");
	}

}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** // 
// ****************************************************************** //

#if WITH_EDITOR
bool AVAReceiverActor::CanEditChange(const UProperty* InProperty) const
{
	// const bool ParentVal = Super::CanEditChange(InProperty);

	// Check manual Adress
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, serverIPAdress))
	{
		return adressSetting == EAdress::manual;
	}

	// Check manual Port
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, serverPort))
	{
		return adressSetting == EAdress::manual;
	}





	return true;
}
#endif
