// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"
#include "VAReceiverActor.h"
#include "VAPlugin.h"

/*
#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "Array.h"
#include "Components/SkeletalMeshComponent.h"
*/

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UVASourceComponent::BeginPlay() {
	Super::BeginPlay();

	TArray<AActor*> recActors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReceiverActor::StaticClass(), recActors);
	AVAReceiverActor* tmp = nullptr;

	for (AActor* iter : recActors) {
		// if there is an ReceiverActor in the Szene
		tmp = dynamic_cast<AVAReceiverActor*> (iter);
		if (tmp != nullptr) {
			VAUtils::logStuff("[UVASourceComponent::BeginPlay()]: AVAReceiver found");
			break;
		}
	}

	// If no Rec Actor found spawn one with default parameters
	if (tmp == nullptr) {
	 	VAUtils::logStuff("[UVASourceComponent::BeginPlay()]: No AVAReceiver found! Spawning one with default values");
		tmp = this->GetWorld()->SpawnActor<AVAReceiverActor>(AVAReceiverActor::StaticClass());
	}

	// If the receiver Actor is initialized but this sound Component not, this Component is spawned at runtime and has to be initialized
	if (tmp->isInitialized() && !initialized) {
		initialize();
	}

}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!FVAPluginModule::getUseVA()) {
		return;
	}

	if (!initialized) {
		VAUtils::openMessageBox("A Sound source is not initialized");
	}

	if (firstTick && FVAPluginModule::getIsMaster()) {
		timeSinceUpdate = 1.0f;
		if (startingPlayAction == EPlayAction::Play) {
			soundSource->playSoundFromSecond(startingTime);
		}
	}
	firstTick = false;


	timeSinceUpdate += DeltaTime;

	if ((movementSetting == EMovement::AttatchToBone || movementSetting == EMovement::MoveWithObject) &&
		timeSinceUpdate > (1.0f / 30.0f)) {

		soundSource->setPos(getPosition());
		soundSource->setRot(getRotation());

		timeSinceUpdate = 0.0f;
	}

}


void UVASourceComponent::initialize()
{

	if (!FVAPluginModule::getUseVA() || initialized) {
		return;
	}

	ownerActor = GetOwner();
	firstTick = true;


	skeletal_mesh_component = dynamic_cast<USkeletalMeshComponent*> (ownerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (movementSetting == EMovement::AttatchToBone) {
		if (skeletal_mesh_component != nullptr
			&& skeletal_mesh_component->DoesSocketExist(FName(*boneName))) {
			VAUtils::logStuff("Bone detected.");
		}
		else {
			VAUtils::openMessageBox("Error: Could not find bone, using MoveWithObject instead.");
			movementSetting = EMovement::MoveWithObject;
		}
	}


	spawnPosition = ownerActor->GetTransform().GetLocation();
	spawnRotation = ownerActor->GetTransform().GetRotation().Rotator();


	soundSource = new VASoundSource(this);

	if (FVAPluginModule::getIsMaster()) {
		switch (directivitySetting) {
		case EDir::DefaultHRIR:
			soundSource->setDirectivity(VADirectivityManager::getDefaultDirectivity());
			break;

		case EDir::manualFile:
			soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByFileName(directivityByFileName));
			break;

		case EDir::phoneme:
			soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByMapping(directivityByMapping));
			break;

		default:
			break;
		}
	}

	VAUtils::logStuff("SoundSourceComponent initialized successfully");

	initialized = true;
}

bool UVASourceComponent::shouldSendCommand()
{
	return (initialized && FVAPluginModule::getUseVA() && FVAPluginModule::getIsMaster());
}


// ****************************************************************** // 
// ******* Interface Sound ****************************************** //
// ****************************************************************** //

bool UVASourceComponent::playSound()
{
	if (!shouldSendCommand()) {
		return false;
	}

	soundSource->playSound();

	return true;
}

bool UVASourceComponent::playSoundFromSecond(float time)
{
	if (!shouldSendCommand()) {
		return false;
	}

	soundSource->playSoundFromSecond(time);

	return true;
}

bool UVASourceComponent::stopSound()
{
	if (!shouldSendCommand()) {
		return false;
	}

	soundSource->stopSound();
	
	return true;
}

bool UVASourceComponent::pauseSound()
{
	if (!shouldSendCommand()) {
		return false;
	}

	soundSource->pauseSound();

	return true;
}

EPlayAction UVASourceComponent::getPlayState()
{
	if (!shouldSendCommand()) {
		return EPlayAction::Stop;
	}

	return VAUtils::VAActionToEPlayAction(soundSource->getPlayState());
}

void UVASourceComponent::muteSound(bool mute_)
{
	if (!shouldSendCommand()) {
		return;
	}

	if (muted == mute_) {
		return;
	}

	soundSource->muteSound(muted);
}



// ****************************************************************** // 
// ******* Sound Pose *********************************************** //
// ****************************************************************** //

FVector UVASourceComponent::getPosition()
{
	FVector pos; 
	switch (movementSetting) {
		case EMovement::MoveWithObject:
			pos = ownerActor->GetTransform().GetLocation();
			break;

		case EMovement::ObjectSpawnPoint:
			pos = spawnPosition;
			break;

		case EMovement::AttatchToBone:
			pos = skeletal_mesh_component->GetSocketLocation(FName(*boneName));
			break;
		
		default:
			pos = FVector::ZeroVector;
			VAUtils::logStuff(FString("[UVASourceComponent::getPosition()]: default"));
			break;
	}

	if (usePoseOffset) {
		pos = pos + offsetPosition;
	}

	return pos;
}

FRotator UVASourceComponent::getRotation()
{
	FRotator rot;
	switch (movementSetting) {
	case EMovement::MoveWithObject:
		rot = ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::ObjectSpawnPoint:
		rot = spawnRotation;
		break;

	case EMovement::AttatchToBone:
		rot = skeletal_mesh_component->GetSocketRotation(FName(*boneName));
		break;

	default:
		VAUtils::logStuff(FString("[UVASourceComponent::getRotation()]: default"));
		rot = FRotator::ZeroRotator;
		break;
	}

	if (usePoseOffset) {
		FQuat quat(rot);
		quat = quat * FQuat(offsetRotation);
		rot = FRotator(quat);
	}

	return rot;
}


// ****************************************************************** // 
// ******* Sound Settings ******************************************* //
// ****************************************************************** //

bool UVASourceComponent::setSoundSourceVisibility(bool vis)
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}
	
	soundSource->setVisibility(vis);

	return true;
}



bool UVASourceComponent::loadSoundFile(FString soundFile_)
{
	if (!shouldSendCommand()) {
		return false;
	}

	return soundSource->loadNewSound(soundFile);
}

bool UVASourceComponent::setSoundFile(FString soundFile_)
{
	if (!shouldSendCommand()) {
		return false;
	}
	
	// If already playing back that sound File
	if (soundFile == soundFile_) {
		soundSource->stopSound();
		return true;
	}

	soundFile = soundFile_;

	return soundSource->setNewSound(soundFile);
}


bool UVASourceComponent::setSoundPower(float power)
{
	if (!shouldSendCommand()) {
		return false;
	}
	
	// If already playing back that sound File
	if (soundPower == power) {
		return true;
	}

	soundPower = power;

	soundSource->setPower(power);

	return true;
}


bool UVASourceComponent::setLoop(bool loop_)
{
	if (!shouldSendCommand()) {
		return false;
	}
	
	// If this setting is already set properly
	if (loop == loop_) {
		return true;
	}

	loop = loop_;


	soundSource->setLoop(loop);

	return true;
}


bool UVASourceComponent::setUsePoseOffset(bool usePoseOffset_)
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}

	// If this setting is already set properly
	if (usePoseOffset == usePoseOffset_) {
		return true;
	}
	
	usePoseOffset = usePoseOffset_;

	soundSource->setPos(getPosition());
	soundSource->setRot(getRotation());

	return true;
}

bool UVASourceComponent::setOffsetPosition(FVector pos_) 
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}

	// If this setting is already set properly
	if (offsetPosition == pos_) {
		return true;
	}

	offsetPosition = pos_;

	soundSource->setPos(getPosition());

	return true;
}

bool UVASourceComponent::setOffsetRotation(FRotator rot_)
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}
	
	// If this setting is already set properly
	if (offsetRotation == rot_) {
		return true;
	}

	offsetRotation = rot_;

	soundSource->setRot(getRotation());

	return true;
}

bool UVASourceComponent::setBoneName(FString boneName_)
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}

	// If this setting is already set properly
	if (boneName == boneName_) {
		return true;
	}

	boneName = boneName_;



	// Check if the bone exists
	if (skeletal_mesh_component != nullptr
		&& skeletal_mesh_component->DoesSocketExist(FName(*boneName_))) {
		boneName = boneName_;
		movementSetting = EMovement::AttatchToBone;
		return true;
	}
	else {
		VAUtils::openMessageBox("[UVASourceComponent::setBoneName(FString)]: Could not find new bone, using old settings instead.");
		return false;
	}

}


bool UVASourceComponent::setMovementSetting(EMovement movementSetting_)
{
	if (!FVAPluginModule::getUseVA()) {
		return false;
	}

	// If this setting is already set properly
	if (movementSetting == movementSetting_) {
		return true;
	}

	movementSetting = movementSetting_;


	soundSource->setPos(getPosition());
	soundSource->setRot(getRotation());

	return true;
}


// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //

bool UVASourceComponent::setDirectivityByMapping(FString phoneme)
{
	if (!shouldSendCommand()) {
		return false;
	}

	directivitySetting = EDir::phoneme;
	directivityByMapping = phoneme;

	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByMapping(phoneme));

	return true;
}

bool UVASourceComponent::setDirectivityByFileName(FString fileName)
{
	if (!shouldSendCommand()) {
		return false;
	}
	
	directivitySetting = EDir::manualFile;
	directivityByFileName = fileName;

	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByFileName(fileName));

	return true;
}


// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

bool UVASourceComponent::getVisibility()
{
	return FVAPluginModule::isInDebugMode();
}

bool UVASourceComponent::getHandleReflections() {
	return handleReflections;
}

float UVASourceComponent::getSoundTimeOffset() {
	return startingTime;
}

FString UVASourceComponent::getBoneName()
{
	if (movementSetting != EMovement::AttatchToBone) {
		VAUtils::logStuff("[UVASourceComponent::getBoneName()]: Movement is not set to AttatchToBone..");
	}
	return boneName;
}

FString UVASourceComponent::getDirectivityFileName()
{
	if (soundSource != nullptr) {
		return soundSource->getDirectivity()->getFileName();
	}

	return "";
}

bool UVASourceComponent::getLoop()
{
	return loop;
}

FString UVASourceComponent::getSoundFile()
{
	return soundFile;
}

float UVASourceComponent::getSoundPower()
{
	return soundPower;
}

// ****************************************************************** // 
// ******* Blueprint Settings *************************************** //
// ****************************************************************** //

#if WITH_EDITOR
bool UVASourceComponent::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, boneName))
	{
		return movementSetting == EMovement::AttatchToBone;
	}

	// Check Directivity Config
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, directivityByFileName))
	{
		return directivitySetting == EDir::manualFile;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, directivityByMapping))
	{
		return directivitySetting == EDir::phoneme;
	}


	return ParentVal;
}
#endif