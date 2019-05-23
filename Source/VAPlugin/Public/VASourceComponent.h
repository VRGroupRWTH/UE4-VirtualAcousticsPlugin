// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAPlugin.h"
#include "CoreMinimal.h"
#include "Array.h"
#include "Components/ActorComponent.h"
#include "VASourceComponent.generated.h"


// struct used to get 
UENUM()
enum EMovement {
	ObjectSpawnPoint,
	MoveWithObject,
	OwnPosition,
	Manual
};

// struct used to get 
UENUM()
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
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float vGainFactor = 1;

	// In- or decrease Gain Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name"))
		FString vSoundName = "Audiofiles/Bauer.wav";

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action"))
		TEnumAsByte<EPlayAction> vAction = EPlayAction::Play;

	// Loop sound?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop"))
		bool vLoop = false;

	// Start at Second x
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Starting Sound at Second x"))
		float vDelay = 0.0f;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings" ,CustomStructureParam = "Objects Spawn Point, Move With Object, Own Position (below)"))
		TEnumAsByte<EMovement> vMovement = EMovement::ObjectSpawnPoint;

	// Manual Position (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position"))
		FVector vPos = FVector(0, 0, 1.7);

	// Manual Rotation (if used)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Rotation"))
		FRotator vRot = FRotator(0, 0, 0);

	// Offset
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset"))
		FVector vOffset = FVector(0, 0, 0);
<<<<<<< HEAD

	// Manual Rotation (if used)
	// UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation"))
	// 	bool vOffsetRot = false;

=======
>>>>>>> ba3f02696ca445d39ecf0fffbdfeb3de8fd5c5a3


public:	
	// Sets default values for this component's properties
	UVASourceComponent();

	bool sendSoundData();

	// Plays Sound // 
	UFUNCTION(BlueprintCallable)
	void playSound();

	// Pauses Sound //
	UFUNCTION(BlueprintCallable)
	void stopSound();

	// Stops Sound //
	UFUNCTION(BlueprintCallable)
	void pauseSound();

	// Update Position // 
	UFUNCTION(BlueprintCallable)
	void updatePosition(FVector vec, FRotator rot);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Link to Owner Actor //
	AActor* ownerActor;

    // Sound ID, used to identify Sound at VA Server //
	int soundID;
    
    // Check if Sound is started, used to start at different time //
	bool started;

	// Check if it is the first tick //
	bool firstTick;
    
    // Check time // 
	float timer;
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
