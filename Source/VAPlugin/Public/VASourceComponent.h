// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VASoundSource.h"								// From VA

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
	ObjectSpawnPoint,
	MoveWithObject
};

UENUM(BlueprintType)
enum EPlayAction {
	Stop = 0,
	Pause = 1,
	Play = 2
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// In- or decrease Gain Factor (linear to power)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Gain Factor",		Category = "General Settings", 
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
		float gainFactor = 1.0f;

	// Name of Sound file. Folder are possible too: "folder/soundfile.wav"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name",			Category = "General Settings"))
		FString soundFile = "WelcomeToVA.wav";

	// Action of the sound source at the first tick
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action",				Category = "General Settings"))
		TEnumAsByte<EPlayAction> startingPlayAction = EPlayAction::Play;

	// Sets Buffer to a specific time stamp when playing back at the first tick (see Action)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]",	Category = "General Settings"))
		float startingTime = 0.0f;

	// Check if the sound should be played back in a loop
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop",				Category = "General Settings"))
		bool loop = false;

	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use reflections?",	Category = "General Settings"))
		bool handleReflections = true;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings",	Category = "Position", 
		CustomStructureParam = "Attatch to a Bone, At Object Spawn Point, Move With the Object"))
		TEnumAsByte<EMovement> movementSetting = EMovement::ObjectSpawnPoint;

	// Use the manual Offset for the position?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use Offset?",		Category = "Position"))
		bool usePoseOffset = true;

	// Offset in Position
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset",				Category = "Position",			EditCondition = "usePoseOffset"))
		FVector offsetPosition = FVector(0, 0, 0);

	// Offset in Rotation
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation",	Category = "Position",			EditCondition = "usePoseOffset"))
		FRotator offsetRotation = FRotator(0, 90, 0);

	// Name of Bone bound to if Position is set to "Attatch to a Bone"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bone Name",			Category = "Position")) 
		FString boneName = "CC_Base_Head";

	// Choose Directivity Setting for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity",		Category = "Directivity"))
		TEnumAsByte<EDir> directivitySetting = EDir::DefaultHRIR;

	// File Name of the Directivity that sould be used
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name",	Category = "Directivity"))		
		FString directivityByFileName = "$(DefaultHRIR)";

	// Directivity that is used by a specific phoneme (see Receiver Actor Directivity Manager)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by phoneme",		Category = "Directivity"))		
		FString directivityByPhoneme = "";





	// Sets default values for this component's properties
	UVASourceComponent();


	// initialize Sound Source with the settings set // 
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

// 	UFUNCTION(BlueprintCallable)										 // TODO! 
// 		void muteSound();												 // TODO! 
// 																		 // TODO! 
// 	UFUNCTION(BlueprintCallable)										 // TODO! 
// 		void unmuteSound();


	// returns the Position minding its setting for location //
	UFUNCTION(BlueprintCallable)
		FVector getPosition();

	// returns the Roatation minding its setting for rotation // 
	UFUNCTION(BlueprintCallable)
		FRotator getRotation();

	// returns the Play Sate in form of a EPlayAction // 
	UFUNCTION(BlueprintCallable)
		EPlayAction getPlayState();

	UFUNCTION(BlueprintCallable)
		void setDirectivityByPhoneme(FString phoneme);

	UFUNCTION(BlueprintCallable)
		void setDirectivityByFileName(FString fileName);

	UFUNCTION(BlueprintCallable)
		void setSoundSourceVisibility(bool vis_);

	UFUNCTION(BlueprintCallable)
		void setGainFactor(float gainFactor_);
	
	UFUNCTION(BlueprintCallable)
		void setSoundFile(FString soundFile_);
	
// 	UFUNCTION(BlueprintCallable)
// 		void setLoop(bool loop_);
// 	
// 	UFUNCTION(BlueprintCallable)
// 		void setUsePoseOffset(bool usePoseOffset_);
// 	
// 	UFUNCTION(BlueprintCallable)
// 		void setOffsetPosition(FVector pos_);
// 	
// 	UFUNCTION(BlueprintCallable)
// 		void setOffsetRotation(FRotator rot_);
// 	
// 	UFUNCTION(BlueprintCallable)
// 		void setBoneName(FString boneName_);






	float getGainFactor();
	float getSoundTimeOffset();
	
	FString getFileName();

	bool getVisibility();
	bool getHandleReflections();
	bool getLoop();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Link to Owner Actor //
	AActor* ownerActor;

	// Check if it is the first tick //
	bool firstTick = true;
    

	// if sound source has run BeginPlay() //
	bool initialized = false;

	// The actual VA Sound Source
	VASoundSource* soundSource;

	// To adapt update rate
	float timeSinceUpdate;

	// Spawn Pos & Rot. Used if ObjectSpawnPoint is selected
	FVector spawnPosition;
	FRotator spawnRotation;

	// skeletal_mesh, used if AttatchToBone is selected
	USkeletalMeshComponent* skeletal_mesh_component;

    // Check time // 
	float timer;


#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const;
#endif
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
