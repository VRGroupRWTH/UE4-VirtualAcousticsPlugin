// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VADefines.h"
#include "VADirectivityManager.h"
#include "VASoundSource.h"

#include "VAPlugin.h"
#include "VAReflectionWall.h"
#include "VASoundSourceRepresentation.h"
#include "CoreMinimal.h"
#include "Array.h"
#include "Components/ActorComponent.h"

#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

#include "VASourceComponent.generated.h"

// struct used to input Directivity via Variable in UE4 Editor
UENUM()
enum EDir {
	DefaultHRIR,
	manualFile,
	phoneme
};

UENUM()
enum EMovement {
	AttatchToBone,
	OwnPosition,
	ObjectSpawnPoint,
	MoveWithObject
};

UENUM(BlueprintType)
enum EPlayAction {
	Play,
	Pause,
	Stop
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// In- or decrease Gain Offset 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor",		Category = "General Settings", 
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
		float vGainFactor = 1.0f;

	// In- or decrease Gain Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name",			Category = "General Settings"))
		FString vSoundName = "WelcomeToVA.wav";

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action",				Category = "General Settings"))
		TEnumAsByte<EPlayAction> vAction = EPlayAction::Play;

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop",				Category = "General Settings"))
		bool vLoop = false;

	// Start at Second x
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]",	Category = "General Settings"))
		float vDelay = 0.0f;

	// Check if reflections should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use reflections?",	Category = "General Settings"))
		bool vHandleReflections = true;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings",	Category = "General Settings", 
		CustomStructureParam = "Attatch to a Bone, Own Position, At Object Spawn Point, Move With the Object"))
		TEnumAsByte<EMovement> vMovement = EMovement::ObjectSpawnPoint;

	// Manual Position (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position",			Category = "Manual Pose"))		// CanEditChange used
		FVector vPos = FVector(0, 0, 1.7);

	// Manual Rotation (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Rotation",			Category = "Manual Pose"))		// CanEditChange used
		FRotator vRot = FRotator(0, 0, 0);

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use Offset?",		Category = "Offset"))
		bool vUseOffset = true;

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset",				Category = "Offset",			EditCondition = "vUseOffset"))
		FVector vOffsetPosition = FVector(0, 0, 0);

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation",	Category = "Offset",			EditCondition = "vUseOffset"))
		FRotator vOffsetRotation = FRotator(0, 90, 0);

	// Name of Bone bound to
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bone Name",			Category = "Attatch to bone")) // CanEditChange used
		FName vBoneName = FName("CC_Base_Head");


	// Choose Directivity File for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity",		Category = "Directivity"))
		TEnumAsByte<EDir> vDirectivity = EDir::DefaultHRIR;

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name",	Category = "Directivity"))		// CanEditChange used
		FString vDirectivityByFileName = "$(DefaultHRIR)";

	// Factor for global output Gain
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by phoneme",		Category = "Directivity"))		// CanEditChange used
		FString vDirectivityByPhoneme = "";





	// Sets default values for this component's properties
	UVASourceComponent();


	// initialize Sound Source // 
	UFUNCTION(BlueprintCallable)
		void initialize();

	// Plays Sound // 
	UFUNCTION(BlueprintCallable)
		void playSound();

	// Plays Sound from second // 
	UFUNCTION(BlueprintCallable)
		void playSoundFromSecond(float time);

	// Pauses Sound //
	UFUNCTION(BlueprintCallable)
		void stopSound();

	// Stops Sound //
	UFUNCTION(BlueprintCallable)
		void pauseSound();

	// returns the Position minding its setting for location //
	UFUNCTION(BlueprintCallable)
		FVector getPosition();

	// returns the Roatation minding its setting for rotation // 
	UFUNCTION(BlueprintCallable)
		FRotator getRotation();



	// NEUE SACHEN F�R �NDERUNG



	FString getFileName();

	bool getHandleReflections();
	bool getLoop();

	float getGainFactor();
	float getSoundTimeOffset();

	void setDirectivityByPhoneme(FString phoneme);

	bool getVisibility();
	void setSoundSourceVisibility(bool vis);

	void setSoundSourcePosition(FVector pos);
	void setSoundSourceRotation(FRotator rot);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Link to Owner Actor //
	AActor* ownerActor;

    // Sound ID, used to identify Sound at VA Server //
	int soundID;
    
    // // Check if Sound is started, used to start at different time //
	// bool started;

	// Check if it is the first tick //
	bool firstTick;

	// Check if Sound data is already sent // 
	bool alreadySent;
    
    // Check time // 
	float timer;

	// // name of face_bone // 
	// FName face_bone_name;

	// skeletal_mesh
	USkeletalMeshComponent* skeletal_mesh_component;

	// // root component
	// USphereComponent* root_component;

	// if sound source has run BeginPlay() //
	bool initialized;

	// Map of all cones that have to be placed at their position
	// TMap<AVAReflectionWall*, FTransform> conesTodo;


	// UPROPERTY(VisibleDefaultsOnly)
	// 	class USphereComponent *sphereComp;
	// 
	// UPROPERTY(VisibleDefaultsOnly)
	// 	TMap<AVAReflectionWall*, class UStaticMeshComponent*> coneMeshMap;
	// 
	// UPROPERTY(VisibleDefaultsOnly)
	// 	TMap<AVAReflectionWall*, AVASoundSourceRepresentation*> sourceReprMap;




	// NEUES ZEUG //
	AVASoundSourceRepresentation* sourceRepr;
	
	VASoundSource* soundSource;

	FVector spawnPosition;
	FRotator spawnRotation;

	float timeSinceUpdate;

	bool isMaster;

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const;
#endif
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
