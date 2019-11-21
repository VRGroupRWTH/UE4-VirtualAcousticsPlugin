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

// bool AVAReceiverActor::vDebugMode;

VADirectivityManager AVAReceiverActor::dirManager;

// Sets default values
AVAReceiverActor::AVAReceiverActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	wallsInitialized = false;

}

void AVAReceiverActor::BeginDestroy()
{
	Super::BeginDestroy();
	FVAPluginModule::resetServer();
	// VAUtils::openMessageBox("In WhenDestroyed");
}

// void AVAReceiverActor::EndPlay() 
// {
// 	VAUtils::openMessageBox("In End Play!!!");
// }

// Called when the game starts or when spawned
void AVAReceiverActor::BeginPlay()
{
	Super::BeginPlay();

	tmpPos = new VAVec3();
	tmpQuat = new VAQuat();

	wallsInitialized = false;
	
	timeSinceUpdate = 0.0f;

	controller = GetWorld()->GetFirstPlayerController();
	
	
#if PLATFORM_WINDOWS
	FString adresse = "localhost";
#else
	FString adresse = "10.0.1.240";
#endif

	
	FVAPluginModule::setScale(vScale);
	
	FVAPluginModule::askForSettings(adresse, vPort);

	isMaster = FVAPluginModule::getIsMaster();

	if (!FVAPluginModule::getUseVA()) {
		return;
	}

	if (isMaster) {
		if (!FVAPluginModule::isConnected()) {
			// Connect to VA Server
			// FVAPluginModule::initializeServer(adresse, vPort);
			FVAPluginModule::connectServer(adresse, vPort);
		}
		else {
			FVAPluginModule::resetServer();
		}
	
		// Initialize Receiver Actor
		receiverID = FVAPluginModule::createNewSoundReceiver(this);
	
	}

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

	// FVAPluginModule::processSoundQueue();

	dirManager.readConfigFile(dirName);

	// FVAPluginModule::readDirFile(dirName);

	// OnDestroyed.AddDynamic(this, WhenDestroyed);
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

// Called every frame
void AVAReceiverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FVAPluginModule::getUseVA() || !isMaster) {
		return;
	}

	timeSinceUpdate += DeltaTime;
	

	if (timeSinceUpdate > (1.0f / 30.0f)) {
		updateVirtualWorldPosition();
		// FVAPluginModule::setSoundReceiverPosition(receiverID, FVector(100,0,0));
		// FVAPluginModule::setSoundReceiverRotation(receiverID, FRotator(0,0,0));
        updateRealWorldPosition();
		trash = FVAPluginModule::getUseVA();
		
		timeSinceUpdate = 0.0f;
	}
}


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
     
    // pos = rightComp->GetRealtiveLocation();
    // rot = rightComp->GetRelativeRotation().Rotator();

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

	// log positions
	// VAUtils::logStuff(FString("RL pos: " + pos.ToString() + "RL rot: " + rot.ToString()));
	
    return FVAPluginModule::setSoundReceiverRealWorldPose(receiverID, pos, rot);

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
	if (vAdressType == EAdress::localhost) {
		return "localhost";
	}
	else if (vAdressType == EAdress::Cave) {
		return "10.0.1.240";
	}

	VAUtils::logStuff("could not evaluate IP Adress. Returning localhost (AVAReceiverActor::getIPAdress())");

	return "localhost";
}

// float AVAReceiverActor::getGainFactor()
// {
// 	return vGainFactor;
// }


VADirectivity* AVAReceiverActor::getDirectvityByPhoneme(FString phoneme) 
{
	return dirManager.getDirectivityByPhoneme(phoneme);
}

TArray<AVAReflectionWall*> AVAReceiverActor::getReflectionWalls()
{
	if (!wallsInitialized) {
		initializeWalls();
	}
	return reflectionWalls;
}


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
