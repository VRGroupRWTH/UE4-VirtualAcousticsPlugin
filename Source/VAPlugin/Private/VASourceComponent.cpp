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
			soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByPhoneme(directivityByPhoneme));
			break;

		default:
			break;
		}
	}

	VAUtils::logStuff("SoundSourceComponent initialized successfully");

	initialized = true;
}

bool UVASourceComponent::hasAccess()
{
	return (initialized && FVAPluginModule::getUseVA() && FVAPluginModule::getIsMaster());
}


// ****************************************************************** // 
// ******* Interface Sound ****************************************** //
// ****************************************************************** //

void UVASourceComponent::playSound()
{
	if (!hasAccess()) {
		return;
	}

	soundSource->playSound();
}

void UVASourceComponent::playSoundFromSecond(float time)
{
	if (!hasAccess()) {
		return;
    }

	soundSource->playSoundFromSecond(time);
}

void UVASourceComponent::stopSound()
{
	if (!hasAccess()) {
		return;
	}

	soundSource->stopSound();
}

void UVASourceComponent::pauseSound()
{
	if (!hasAccess()) {
		return;
	}

	soundSource->pauseSound();
}

EPlayAction UVASourceComponent::getPlayState()
{
	if (!hasAccess()) {
		return EPlayAction::Stop;
	}

	return VAUtils::VAActionToEPlayAction(soundSource->getPlayState());
}

void UVASourceComponent::muteSound(bool mute_)
{
	if (muted == mute_) {
		return;
	}

	if (!hasAccess()) {
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

void UVASourceComponent::setSoundSourceVisibility(bool vis)
{
	if (!FVAPluginModule::getUseVA()) {
		return;
	}
	soundSource->setVisibility(vis);
}

// void UVASourceComponent::setGainFactor(float gainFactor_)
// {
// 	// TODO: setGainFactor
// 	VAUtils::logStuff("UVASourceComponent::setGainFactor not working yet!!", true);
// 	return;
// }

bool UVASourceComponent::loadSoundFile(FString soundFile_)
{
	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return false;
	}

	return soundSource->loadNewSound(soundFile);
}

bool UVASourceComponent::setSoundFile(FString soundFile_)
{
	// If already playing back that sound File
	if (soundFile == soundFile_) {
		if(hasAccess())
			soundSource->stopSound();
		return false;
	}

	soundFile = soundFile_;
	
	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return false;
	}

	return soundSource->setNewSound(soundFile);	
}


void UVASourceComponent::setLoop(bool loop_)
{
	// If this setting is already set properly
	if (loop == loop_) {
		return;
	}

	loop = loop_;

	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}

	soundSource->setLoop(loop);
}


void UVASourceComponent::setUsePoseOffset(bool usePoseOffset_) 
{
	// If this setting is already set properly
	if (usePoseOffset == usePoseOffset_) {
		return;
	}
	
	usePoseOffset = usePoseOffset_;
	
	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}
	
	soundSource->setPos(getPosition());
	soundSource->setRot(getRotation());
}

void UVASourceComponent::setOffsetPosition(FVector pos_) 
{
	// If this setting is already set properly
	if (offsetPosition == pos_) {
		return;
	}

	offsetPosition = pos_;

	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}

	soundSource->setPos(getPosition());
}

void UVASourceComponent::setOffsetRotation(FRotator rot_)
{
	// If this setting is already set properly
	if (offsetRotation == rot_) {
		return;
	}

	offsetRotation = rot_;

	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}

	soundSource->setRot(getRotation());
}

void UVASourceComponent::setBoneName(FString boneName_)
{
	// If this setting is already set properly
	if (boneName == boneName_) {
		return;
	}

	boneName = boneName_;

	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}

	// Check if the bone exists
	if (skeletal_mesh_component != nullptr
		&& skeletal_mesh_component->DoesSocketExist(FName(*boneName_))) {
		boneName = boneName_;
		movementSetting = EMovement::AttatchToBone;
	}
	else {
		VAUtils::openMessageBox("[UVASourceComponent::setBoneName(FString)]: Could not find new bone, using old settings instead.");
	}
}


void UVASourceComponent::setMovementSetting(EMovement movementSetting_)
{
	// If this setting is already set properly
	if (movementSetting == movementSetting_) {
		return;
	}

	movementSetting = movementSetting_;

	if (!initialized || !FVAPluginModule::getIsMaster()) {
		return;
	}

	soundSource->setPos(getPosition());
	soundSource->setRot(getRotation());
}


// ****************************************************************** // 
// ******* Directivity stuff **************************************** //
// ****************************************************************** //

void UVASourceComponent::setDirectivityByPhoneme(FString phoneme)
{

	directivitySetting = EDir::phoneme;
	directivityByPhoneme = phoneme;

	if (!hasAccess()) {
		return;
	}

	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByPhoneme(phoneme));
}

void UVASourceComponent::setDirectivityByFileName(FString fileName)
{
	
	directivitySetting = EDir::manualFile;
	directivityByFileName = fileName;

	if (!hasAccess()) {
		return;
	}

	soundSource->setDirectivity(AVAReceiverActor::getCurrentReceiverActor()->getDirectivityByFileName(fileName));
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

float UVASourceComponent::getGainFactor() {
	return gainFactor;
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
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, directivityByPhoneme))
	{
		return directivitySetting == EDir::phoneme;
	}


	return ParentVal;
}
#endif