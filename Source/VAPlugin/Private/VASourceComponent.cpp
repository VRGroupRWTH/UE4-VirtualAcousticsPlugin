// Fill out your copyright notice in the Description page of Project Settings.

#include "VASourceComponent.h"

// Sets default values for this component's properties
UVASourceComponent::UVASourceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// root_component = CreateDefaultSubobject<USphereComponent>(TEXT("RootCmp"));
	// 
	// skeletal_mesh_component = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshCmp"));
	// skeletal_mesh_component->SetupAttachment(root_component);

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->bHiddenInGame = true;
	sphereComp->Mobility = EComponentMobility::Movable;
	sphereComp->SetRelativeScale3D(FVector(1, 1, 1));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));
	


	// TArray<AActor*> wallsA;
	// UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AVAReflectionWall::StaticClass(), wallsA);
	// TArray<AVAReflectionWall*> walls;
	// for (AActor* actor : wallsA) {
	// 	walls.Add((AVAReflectionWall*)actor);
	// }
	// 
	// for (auto wall : walls)
	// {
	// 	VAUtils::openMessageBox("In walls loop");
	// 	class UStaticMeshComponent *coneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	// 	coneMesh->AttachTo(sphereComp);
	// 
	// 	if (ConeMeshAsset.Succeeded()) {
	// 		VAUtils::openMessageBox("ConeMeshAsset succeded");
	// 		coneMesh->SetStaticMesh(ConeMeshAsset.Object);
	// 		coneMesh->SetMobility(EComponentMobility::Movable);
	// 		coneMesh->SetWorldScale3D(FVector(0.8f));
	// 		coneMesh->SetVisibility(true); // FVAPluginModule::isInDebugMode());
	// 		coneMesh->SetWorldLocation(FVector(100,100,100));
	// 		// coneMesh->SetWorldRotation(rot);
	// 		coneMeshMap.Add(wall, coneMesh);
	// 	}
	// 
	// }
	// 
	// initialized = true;
	// 
	// for (auto& Elem : conesTodo)
	// {
	// 	setReflectedSourceRepresentation(Elem.Key, Elem.Value.GetLocation(), Elem.Value.GetRotation().Rotator());
	// }


}


// Called when the game starts
void UVASourceComponent::BeginPlay()
{
	Super::BeginPlay();

	// USphereComponent* root_component;skeletal_mesh_component->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(idle_animation, default_slot_name, 0.0f, 0.0f, 1.0f, 1000);

}


bool UVASourceComponent::sendSoundData()
{
	if (alreadySent) {
		VAUtils::logStuff("Send Sound data for multiple times");
		return false;
	}

	alreadySent = true;

	FVector pos;
	FRotator rot;

	pos = getPosition();
	rot = getRotation();

	/*
	int vActionP;
	switch (vAction)
	{
		case EPlayAction::Play :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_PLAY;
			break;

		case EPlayAction::Pause :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_PAUSE;
			break;

		case EPlayAction::Stop :
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
			break;

		default: 
			vActionP = IVAInterface::VA_PLAYBACK_ACTION_STOP;
	}
	*/

	VAUtils::logStuff(FString("pos for ini: " + pos.ToString()));

	// soundID = FVAPluginModule::initializeSound(vSoundName, pos, rot, vGainOffset, vLoop, vDelay, vActionP);
	soundID = FVAPluginModule::initializeSoundWithReflections(vSoundName, pos, rot, vGainFactor * vGainFactor, vLoop, vDelay, IVAInterface::VA_PLAYBACK_ACTION_STOP, this);

	return true;
}

void UVASourceComponent::initialize()
{
	ownerActor = GetOwner();
	started = false;
	firstTick = true;
	alreadySent = false;

	skeletal_mesh_component = dynamic_cast<USkeletalMeshComponent*> (ownerActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	// face_bone_name = "CC_Base_FacialBone";
	face_bone_name = "CC_Base_Head";

	if (skeletal_mesh_component != nullptr && skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
		vMovement = EMovement::Human;
		VAUtils::logStuff("Human detected");
	}


	if (FVAPluginModule::isConnected()) {
		sendSoundData();
	}
	else {
		FVAPluginModule::enqueueSound(this);
	}

}

void UVASourceComponent::playSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
}

void UVASourceComponent::stopSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::pauseSound()
{
	FVAPluginModule::setSoundAction(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::updatePosition(FVector vec, FRotator rot)
{
	FVAPluginModule::updateSourcePos(soundID, vec, rot);
}

void UVASourceComponent::playSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
}

void UVASourceComponent::stopSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::pauseSoundWithReflections()
{
	FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_STOP);
}

void UVASourceComponent::updatePositionWithReflections(FVector vec, FRotator rot)
{
	FVAPluginModule::updateSourcePosWithReflections(soundID, vec, rot);
}

FVector UVASourceComponent::getPosition()
{
	switch (vMovement) {
		case EMovement::MoveWithObject:
			return ownerActor->GetTransform().GetLocation() + vOffset;
			break;

		case EMovement::ObjectSpawnPoint:
			return ownerActor->GetTransform().GetLocation() + vOffset;
			break;

		case EMovement::OwnPosition:
			return vPos + vOffset;
			break;

		case EMovement::Human:
			if (!skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
				VAUtils::logStuff(FString("Could not find face_bone in getPosition"));
				break;
			}
			return skeletal_mesh_component->GetSocketLocation(face_bone_name) + vOffset;
			break;
		
		default:
			VAUtils::logStuff(FString("default: in getPosition"));
			break;
	}

	return FVector::ZeroVector;
}

FRotator UVASourceComponent::getRotation()
{
	FRotator rot;
	switch (vMovement) {
	case EMovement::MoveWithObject:
		rot = ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::ObjectSpawnPoint:
		rot = ownerActor->GetTransform().GetRotation().Rotator();
		break;

	case EMovement::OwnPosition:
		rot = vRot;
		break;

	case EMovement::Human:
		if (!skeletal_mesh_component->DoesSocketExist(face_bone_name)) {
			VAUtils::logStuff(FString("Could not find face_bone in getRotation"));
			break;
		}
		rot = skeletal_mesh_component->GetSocketRotation(face_bone_name) + FRotator(0, 90, 0);
		break;

	default:
		VAUtils::logStuff(FString("default: in getRotation"));
		rot = FRotator::ZeroRotator;
		break;
	}

	FString text = "Rotation of Source is: ";
	text.Append(FString::FromInt(rot.Roll)).Append("/").Append(FString::FromInt(rot.Pitch)).Append("/").Append(FString::FromInt(rot.Yaw));
	VAUtils::logStuff(text);
	
	return rot;
}


bool UVASourceComponent::setDirectivityWithReflections_Phoneme(FString phoneme)
{
	return FVAPluginModule::setSourceDirectivityWithReflections_Phoneme(soundID, phoneme);
}

bool UVASourceComponent::setDirectivity_Phoneme(FString phoneme)
{
	return FVAPluginModule::setSourceDirectivity_Phoneme(soundID, phoneme);
}


// Called every frame
void UVASourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (firstTick) {
		firstTick = false;
		if (vAction == EPlayAction::Play) {
			FVAPluginModule::setSoundActionWithReflections(soundID, IVAInterface::VA_PLAYBACK_ACTION_PLAY);
			started = true;
		}
	}

	
	// if (!started) {
	// 	timer += DeltaTime;
	// 	if (timer > vDelay) {
	// 		playSoundWithReflections();
	// 		started = true;
	// 	}
	// }
	

	// update if object is / could be moving
	if(vMovement == EMovement::Human || vMovement == EMovement::MoveWithObject) {
		FVector pos = getPosition();
		FRotator rot = getRotation();
		FVAPluginModule::updateSourcePosWithReflections(soundID, pos, rot, this);
		
		if (FVAPluginModule::isInDebugMode()) {
			setSourceRepresentation();
			// sourceComp->setReflectedSourceReprVisibility(wall, true);
		}

	}
}

bool UVASourceComponent::setReflectedSourceRepresentation(AVAReflectionWall *wall, FVector pos, FRotator rot)
{
	/*
	// sceneComp = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	// sceneComp->bHiddenInGame = !FVAPluginModule::isInDebugMode();
	// sceneComp->Mobility = EComponentMobility::Movable;
	// sceneComp->SetRelativeScale3D(FVector(1, 1, 1));

	// class UStaticMeshComponent* NewComponent = ConstructObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), this, TEXT("SphereComp"));

	// NewComponent->RegisterComponent();
	// NewComponent->OnComponentCreated(); // Might need this line, might not.
	// NewComponent->AttachTo(GetRootComponent(), SocketName);
	// 
	// class UStaticMeshComponent *coneMesh = NewObject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	// coneMesh->AttachTo(sphereComp);
	// static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));
	// 
	// if (SphereMeshAsset.Succeeded()) {
	// 	coneMesh->SetStaticMesh(SphereMeshAsset.Object);
	// 	coneMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	// 	coneMesh->SetWorldLocation(pos);
	// 	coneMesh->SetWorldRotation(rot);
	// 	coneMesh->SetWorldScale3D(FVector(0.8f));
	// 	coneMesh->SetVisibility(FVAPluginModule::isInDebugMode());
	// }
	
	// coneMeshMap.Add(wall, coneMesh);
	// class UStaticMeshComponent tmpMesh = dynamic_cast<UStaticMeshComponent>(coneMeshMap.Find(wall));

	*/

	ASoundSourceRepresentation* tmp;

	if (sourceReprMap.Contains(wall)) {
		tmp = sourceReprMap[wall];

		tmp->setPos(pos);
		tmp->setRot(rot);

	}
	else {
		// If there is no Source Representation for that wall yet
		tmp = GetWorld()->SpawnActor<ASoundSourceRepresentation>(ASoundSourceRepresentation::StaticClass());

		tmp->setPos(pos);
		tmp->setRot(rot);
		tmp->setVis(false);

		sourceReprMap.Add(wall, tmp);
	}
	

	return true;

	/*
	// if (initialized == false) {
	// 	FTransform trans;
	// 	trans.SetLocation(pos);
	// 	trans.SetRotation(rot.Quaternion());
	// 	conesTodo.Add(wall, trans);
	// 	return true;
	// }


	// class UStaticMeshComponent *tmpMesh = *(coneMeshMap.Find(wall));

	//tmpMesh->
	// tmpMesh->SetWorldLocation(pos); 
	// tmpMesh->SetWorldRotation(rot);

	
	// return true;

	// class UStaticMeshComponent *coneMesh;
	// coneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	// coneMesh->SetStaticMesh(coneStaticMesh)
	// coneMash->AttachTo(sceneComp);
	// coneMash->
	//coneMeshMap.Add(*wall, comp);

	*/
}

bool UVASourceComponent::setSourceRepresentation()
{
	if (sourceRepr == nullptr) {
		sourceRepr = GetWorld()->SpawnActor<ASoundSourceRepresentation>(ASoundSourceRepresentation::StaticClass());
		sourceRepr->setVis(false);
	}
	sourceRepr->setPos(getPosition());
	sourceRepr->setRot(getRotation());
	return true;
}

bool UVASourceComponent::setReflectedSourceReprVisibility(AVAReflectionWall * wall, bool visibility)
{
	ASoundSourceRepresentation* tmp;

	if (sourceReprMap.Contains(wall)) {
		tmp = sourceReprMap[wall];
		tmp->setVis(visibility);
		return true;
	}

	return false;
}

bool UVASourceComponent::setSourceReprVisibility(bool visibility)
{
	// Check if there is no Source Representation yet
	if (sourceRepr == nullptr) {
		return false;
	}

	sourceRepr->setVis(visibility);
	
	return true;
}


