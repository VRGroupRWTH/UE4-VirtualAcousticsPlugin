// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"
#include "VASoundSource.h"

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	initialized = false;
	firstTick = true;
}


// Called when the game starts
void UVASourceComponent::BeginPlay() {
	Super::BeginPlay();
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

	if (firstTick && isMaster) {
		timeSinceUpdate = 1.0f;
		if (vAction == EPlayAction::Play) {
			soundSource->playSound();
		}
	}
	firstTick = false;


	timeSinceUpdate += DeltaTime;

	if ((vMovement == EMovement::AttatchToBone || vMovement == EMovement::MoveWithObject) && 
		timeSinceUpdate > (1.0f / 30.0f)) {

		// Checks himself if only updating graphical (in every node, not just master) or send information
		
		soundSource->setPos(getPosition());
		soundSource->setRot(getRotation());
		
		// soundSource->setPos(FVector(0, 0, 0));
		// soundSource->setRot(FRotator(0, 0, 0));

		timeSinceUpdate = 0.0f;
	}

}




bool UVASourceComponent::sendSoundData()
{
	/*
	if (alreadySent) {
		VAUtils::logStuff("Send Sound data for multiple times");
		return false;
	}

	alreadySent = true;

	FVector pos;
	FRotator rot;

	pos = getPosition();
	rot = getRotation();

	
	// int vActionP;
	// switch (vAction)
	// {
	// 	case EPlayAction::Play :
	// 		vActionP = IVAInterface::VA_PLAYBACK_ACTION_PLAY;
	// 		break;
	// 
	// 	case EPlayAction::Pause :
	// 		vActionP = IVAInterface::VA_PLAYBACK_ACTION_PAUSE;
	// 		break;
	// 
	// 	case EPlayAction::Stop :
	// 		vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
	// 		break;
	// 
	// 	default: 
	// 		vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
	// }
	

	VAUtils::logStuff(FString("pos for ini: " + pos.ToString()));

	// soundID = FVAPluginModule::initializeSound(vSoundName, pos, rot, vGainOffset, vLoop, vDelay, vActionP);
	// soundID = FVAPluginModule::initializeSoundWithReflections(vSoundName, pos, rot, vGainFactor * vGainFactor, vLoop, vDelay, IVAInterface::VA_PLAYBACK_ACTION_STOP, this);


	// VASoundSource* a = new VASoundSource();
	soundSource = new VASoundSource(this);
	return true;
	*/
	return false;
}

void UVASourceComponent::initialize()
{
	ownerActor = GetOwner();
	firstTick = true;

	isMaster = FVAPluginModule::getIsMaster();

	skeletal_mesh_component = dynamic_cast<USkeletalMeshComponent*> (ownerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	
	if (vMovement == EMovement::AttatchToBone) {
		if (skeletal_mesh_component != nullptr 
			&& skeletal_mesh_component->DoesSocketExist(vBoneName)) {
			VAUtils::logStuff("Bone detected.");
		}
		else {
			VAUtils::openMessageBox("Error: Could not find bone, using MoveWithObject instead.");
			vMovement = EMovement::MoveWithObject;
		}
	}
	
	soundSource = new VASoundSource(this);
	// soundSource->setVisibility(FVAPluginModule::isInDebugMode());

	initialized = true;

}

void UVASourceComponent::playSound()
{
	soundSource->playSound();
}

void UVASourceComponent::playSoundFromSecond(float time)
{
	soundSource->playSoundFromSecond(time);
}

void UVASourceComponent::stopSound()
{
	soundSource->stopSound();
}

void UVASourceComponent::pauseSound()
{
	soundSource->pauseSound();
}

/*
void UVASourceComponent::updatePosition(FVector pos, FRotator rot)
{
	soundSource->setPos(pos);
	soundSource->setRot(rot);
}
*/

void UVASourceComponent::setSoundSourcePosition(FVector pos) {
	soundSource->setPos(pos);
}

void UVASourceComponent::setSoundSourceRotation(FRotator rot) {
	soundSource->setRot(rot);
}


FVector UVASourceComponent::getPosition()
{
	FVector pos; 
	switch (vMovement) {
		case EMovement::MoveWithObject:
			VAUtils::logStuff("getPos: MoveWithObject");
			pos = ownerActor->GetTransform().GetLocation() + vOffset;
			break;

		case EMovement::ObjectSpawnPoint:
			VAUtils::logStuff("getPos: ObjectSpawnPoint");
			pos = spawnPosition + vOffset;
			break;

		case EMovement::OwnPosition:
			VAUtils::logStuff("getPos: OwnPosition");
			pos = vPos + vOffset;
			break;

		case EMovement::AttatchToBone:
			VAUtils::logStuff("getPos: AttatchToBone");
			// if (!skeletal_mesh_component->DoesSocketExist(vBoneName)) {
			// 	VAUtils::logStuff(FString("Could not find vBoneName in getPosition"));
			// 	break;
			// }
			pos = skeletal_mesh_component->GetSocketLocation(vBoneName) + vOffset;
			break;
		
		default:
			pos = FVector::ZeroVector;
			VAUtils::logStuff("getPos: default");
			VAUtils::logStuff(FString("default: in getPosition"));
			break;
	}

	return pos;
}

FRotator UVASourceComponent::getRotation()
{
	FRotator rot;
	switch (vMovement) {
	case EMovement::MoveWithObject:
		rot = ownerActor->GetTransform().GetRotation().Rotator() + vOffsetRotation;
		break;

	case EMovement::ObjectSpawnPoint:
		rot = spawnRotation + vOffsetRotation;
		break;

	case EMovement::OwnPosition:
		rot = vRot + vOffsetRotation;
		break;

	case EMovement::AttatchToBone:
		// if (!skeletal_mesh_component->DoesSocketExist(vBoneName)) {
		// 	VAUtils::logStuff(FString("Could not find vBoneName in getRotation"));
		// 	break;
		// }
		rot = skeletal_mesh_component->GetSocketRotation(vBoneName) + vOffsetRotation;
		break;

	default:
		VAUtils::logStuff(FString("default: in getRotation"));
		rot = FRotator::ZeroRotator;
		break;
	}


	return rot;
}


void UVASourceComponent::setDirectivityByPhoneme(FString phoneme)
{
	VADirectivity* dir = AVAReceiverActor::getDirectvityByPhoneme(phoneme);
	soundSource->setDirectivity(dir);
}


bool UVASourceComponent::getVisibility()
{
	return FVAPluginModule::isInDebugMode();
}

void UVASourceComponent::setSoundSourceVisibility(bool vis)
{
	soundSource->setVisibility(vis);
}

FString UVASourceComponent::getFileName() {
	return vSoundName;
}

bool UVASourceComponent::getHandleReflections() {
	return vHandleReflections;
}

bool UVASourceComponent::getLoop() {
	return vLoop;
}

float UVASourceComponent::getGainFactor() {
	return vGainFactor;
}

float UVASourceComponent::getSoundTimeOffset() {
	return vDelay;
}


bool UVASourceComponent::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	// Check manual Position
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vPos))
	{
		return vMovement == EMovement::OwnPosition;
	}

	// Check manual Rotation
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vRot))
	{
		return vMovement == EMovement::OwnPosition;
	}

	// Check Bone Name
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UVASourceComponent, vBoneName))
	{
		return vMovement == EMovement::AttatchToBone;
	}



	return ParentVal;
}
