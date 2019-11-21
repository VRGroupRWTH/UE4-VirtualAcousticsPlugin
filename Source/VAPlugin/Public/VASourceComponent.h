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


UENUM()
enum EMovement {
	AttatchToBone,
	OwnPosition,
	ObjectSpawnPoint,
	MoveWithObject
};

// struct used to get 
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


	// In- or decrease Gain Offset 
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor",		Category = "General Settings", 
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
		float vGainFactor = 1.0f;

	// In- or decrease Gain Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name",			Category = "General Settings"))
		FString vSoundName = "Audiofiles/Bauer.wav";

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
		FVector vOffset = FVector(0, 0, 0);

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation",	Category = "Offset",			EditCondition = "vUseOffset"))
		FRotator vOffsetRotation = FRotator(0, 90, 0);

	// Name of Bone bound to
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bone Name",			Category = "Attatch to bone")) // CanEditChange used
		FName vBoneName = FName("CC_Base_Head");

	// UPROPERTY(VisibleDefaultsOnly)
	// 	class USceneComponent* sceneComp;
	// 
	// UPROPERTY(VisibleDefaultsOnly)
	// 	TMap<AVAReflectionWall*, class UStaticMeshComponent*> coneMeshMap;

	//UPROPERTY(VisibleDefaultsOnly)
	//	TMap<AVAReflectionWall*, class UStaticMeshComponent*> sphereMeshMapBig;
	//
	//UPROPERTY(VisibleDefaultsOnly)
	//	TMap<AVAReflectionWall*, class UStaticMeshComponent*> sphereMeshMapSmall;

	// Manual Rotation (if used)
	// UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation"))
	// 	bool vOffsetRot = false;



public:	
	// Sets default values for this component's properties
	UVASourceComponent();

	bool sendSoundData();

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

// 	// Update Position // 
// 	UFUNCTION(BlueprintCallable)
// 		void updatePosition(FVector vec, FRotator rot);

// 	// Plays Sound With Reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		void playSoundWithReflections();
// 
//  	// Plays Sound starting at second x With Reflections // 
//  	UFUNCTION(BlueprintCallable)
//  		void playSoundFromSecondWithReflections(float time);
// 
// 	// Pauses Sound With Reflections //
// 	UFUNCTION(BlueprintCallable)
// 		void stopSoundWithReflections();
// 
// 	// Stops Sound With Reflections //
// 	UFUNCTION(BlueprintCallable)
// 		void pauseSoundWithReflections();
// 
// 	// Update Position With Reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		void updatePositionWithReflections(FVector vec, FRotator rot);

	// returns the Position minding its setting for location //
	UFUNCTION(BlueprintCallable)
		FVector getPosition();

	// returns the Roatation minding its setting for rotation // 
	UFUNCTION(BlueprintCallable)
		FRotator getRotation();

// 	// set directivity of sound source with reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setDirectivityWithReflections_Phoneme(FString phoneme);
// 
// 	// set directivity of sound source with reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setDirectivity_Phoneme(FString phoneme);

// 	// set directivity of sound source with reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setReflectedSourceRepresentation(AVAReflectionWall *wall, FVector pos, FRotator rot);
// 
// 	// set directivity of sound source with reflections // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setSourceRepresentation();

// 	// sets visibility of sound source representation // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setReflectedSourceReprVisibility(AVAReflectionWall *wall, bool visibility);
// 
// 	// sets visibility of sound source representation // 
// 	UFUNCTION(BlueprintCallable)
// 		bool setSourceReprVisibility(bool visibility);




	// NEUE SACHEN FÜR ÄNDERUNG



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

	virtual bool CanEditChange(const UProperty* InProperty) const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
