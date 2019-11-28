// Fill out your copyright notice in the Description page of Project Settings.
// what's up?

#include "VAReceiverActor.h"
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

// bool AVAReceiverActor::vDebugMode;

VADirectivityManager AVAReceiverActor::dirManager;

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

	// Ask if used or not
	FVAPluginModule::askForSettings(getIPAdress(), getPort(), vAskForDebugMode);

	if (!FVAPluginModule::getUseVA()) {
		return;
	}

	// General stuff 
	tmpPos = new VAVec3();
	tmpQuat = new VAQuat();

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

	FVAPluginModule::setScale(vScale);
	

	isMaster = FVAPluginModule::getIsMaster();

	if (!FVAPluginModule::getUseVA()) {
		return;
	}

	if (isMaster) {
		if (!FVAPluginModule::isConnected()) {
			// Connect to VA Server
			// FVAPluginModule::initializeServer(adresse, vPort);
			FVAPluginModule::connectServer(getIPAdress(), getPort());
		}
		else {
			FVAPluginModule::resetServer();
		}
	
	
	}
	// Initialize Receiver Actor
	receiverID = FVAPluginModule::createNewSoundReceiver(this);

	// Initialize Walls for Sound Reflection
	if (!wallsInitialized) {
		initializeWalls();
	}

	// FVAPluginModule::initializeWalls(walls);

	// // Initialize Sounds that could not have been processed earlier because of the missing connection to the VA Server
	// FVAPluginModule::processSoundQueue();

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

	dirManager.readConfigFile(dirName);

}

void AVAReceiverActor::BeginDestroy()
{
	Super::BeginDestroy();
	FVAPluginModule::resetServer();

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

	if (!FVAPluginModule::getUseVA() || !isMaster) {
		return;
	}

	timeSinceUpdate += DeltaTime;
	totalTime += DeltaTime;

	if (timeSinceUpdate > (1.0f / 30.0f)) {
		updateVirtualWorldPosition();
		// FVAPluginModule::setSoundReceiverPosition(receiverID, FVector(100,0,0));
		// FVAPluginModule::setSoundReceiverRotation(receiverID, FRotator(0,0,0));
		updateRealWorldPosition();
		trash = FVAPluginModule::getUseVA();

		timeSinceUpdate = 0.0f;
	}

	if (totalTime > 8.0f) {
		runOnAllNodes("debugMode = false");
		timeSinceUpdate = -100000.0f;
	}
}


// ****************************************************************** // 
// ******* Position updates ***************************************** //
// ****************************************************************** //

bool AVAReceiverActor::updateVirtualWorldPosition()
{
	controller->GetPlayerViewPoint(tmpPosF, tmpRotF);

	FString text = "Position of Receiver (id: " + FString::FromInt(receiverID) + ") is: ";
	text.Append(FString::FromInt(tmpPosF.X)).Append("/").Append(FString::FromInt(tmpPosF.Y)).Append("/").Append(FString::FromInt(tmpPosF.Z));
	VAUtils::logStuff(text);

	text = "Rotation of Receiver (id: " + FString::FromInt(receiverID) + ") is: ";
	text.Append(FString::FromInt(tmpRotF.Roll)).Append("/").Append(FString::FromInt(tmpRotF.Pitch)).Append("/").Append(FString::FromInt(tmpRotF.Yaw));
	
	VAUtils::logStuff(text);

	FVAPluginModule::setSoundReceiverPosition(receiverID, tmpPosF);
	FVAPluginModule::setSoundReceiverRotation(receiverID, tmpRotF);

	// FVAPluginModule::updateReceiverPos(tmpPosF, tmpRotF);

	return false;
}

bool AVAReceiverActor::updateRealWorldPosition()
{
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

	FString name_shutter = "shutter_glasses";
	FVector  shutter; 
	FRotator shutterRot;
	
	FString name_origin = "cave_origin";
	FVector  origin;
	FRotator originRot;

	UClass* component_class = UDisplayClusterSceneComponent::StaticClass();

	auto parent_vec = vr_pawn->GetComponentsByClass(component_class);
	bool suc1 = false;
	bool suc2 = false;

	for (auto parent : parent_vec) {
		if (parent->GetName() == FString(name_shutter)) {
			auto tmp = dynamic_cast<USceneComponent*>(parent);
			
			shutter = tmp->GetComponentLocation();
			shutterRot = tmp->GetComponentRotation();
			suc1 = true;
		}
		else if (parent->GetName() == FString(name_origin)) {
			auto tmp = dynamic_cast<USceneComponent*>(parent);

			origin = tmp->GetComponentLocation();
			originRot = tmp->GetComponentRotation();
			suc2 = true;
		}
	}

	// check if both is found
	if (!(suc1 && suc2)) {
		return false;
	}

	// calculate positions
	FVector pos = shutter - origin;
	FRotator rot = shutterRot - originRot;

	// VAUtils::logStuff(FString("RL pos: " + pos.ToString() + "RL rot: " + rot.ToString()));
	
    return FVAPluginModule::setSoundReceiverRealWorldPose(receiverID, pos, rot);
}


// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

float AVAReceiverActor::getScale()
{
	return vScale;
}

FString AVAReceiverActor::getIPAdress()
{

	switch (vAdressType)
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
			return vAdress;
			break;
		default :
			break;
			
	}

	VAUtils::logStuff("Error in AVAReceiverActor::getIPAdress()");

	return FString("localhost");
}

int AVAReceiverActor::getPort()
{
	switch (vAdressType)
	{
		case EAdress::automatic : 
		case EAdress::Cave :
		case EAdress::localhost :
			return 12340;
			break;
		case EAdress::manual:
			return vPort;
			break;
		default:
			break;
	}

	VAUtils::logStuff("Error in AVAReceiverActor::getPort()");

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
		}
		else {
			// else create a cluster event to react to
			FDisplayClusterClusterEvent cluster_event;
			cluster_event.Name = command;
			Manager->EmitClusterEvent(cluster_event, true);
		}
	}

}

void AVAReceiverActor::HandleClusterEvent(const FDisplayClusterClusterEvent & Event)
{
	handleClusterCommand(Event.Name);

}

void AVAReceiverActor::handleClusterCommand(FString command)
{
	if (command == "useVA = false") {
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
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, vAdress))
	{
		return vAdressType == EAdress::manual;
	}

	// Check manual Port
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, vPort))
	{
		return vAdressType == EAdress::manual;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, vDirectivityByFileName))
	{
		return vDirectivity == EDir::manualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AVAReceiverActor, vDirectivityByPhoneme))
	{
		return vDirectivity == EDir::phoneme;
	}



	return true;
}
#endif
