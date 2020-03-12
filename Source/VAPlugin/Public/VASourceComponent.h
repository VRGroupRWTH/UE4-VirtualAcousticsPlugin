// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VASoundSource.h"								// From VA

#include "VASourceComponent.generated.h"

// struct used to input Directivity via Variable in UE4 Editor
UENUM()
enum EDir
{
	DefaultHRIR,
	manualFile,
	phoneme,
	none
};

UENUM()
enum EMovement
{
	AttatchToBone,
	ObjectSpawnPoint,
	MoveWithObject
};

UENUM(BlueprintType)
enum EPlayAction
{
	Stop = 0,
	Pause = 1,
	Play = 2
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	// Having AVAReceiverActor as friend to be able to call the initialize function from AVAReceiverActor::BeginPlay()
	friend class AVAReceiverActor;


	// Name of Sound file. Folder are possible too: "folder/soundfile.wav"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Name", Category = "General Settings"))
	FString soundFile = "WelcomeToVA.wav";

	// Action of the sound source at the first tick
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action", Category = "General Settings"))
	TEnumAsByte<EPlayAction> startingPlayAction = Stop;

	// Sets Buffer to a specific time stamp when playing back at the first tick (see Action)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]", Category = "General Settings"))
	float startingTime = 0.0f;

	// Check if the sound should be played back in a loop
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop", Category = "General Settings"))
	bool loop = false;

	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use reflections?", Category = "General Settings"))
	bool handleReflections = true;

	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Power", Category = "General Settings",
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
	float soundPower = 0.0316227749f;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings", Category = "Position",
		CustomStructureParam = "Attatch to a Bone, At Object Spawn Point, Move With the Object"))
	TEnumAsByte<EMovement> movementSetting = ObjectSpawnPoint;

	// Use the manual Offset for the position?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use Offset?", Category = "Position"))
	bool usePoseOffset = false;

	// Offset in Position
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset", Category = "Position", EditCondition = "usePoseOffset"))
	FVector offsetPosition = FVector(0, 0, 0);

	// Offset in Rotation
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation", Category = "Position", EditCondition =
		"usePoseOffset"))
	FRotator offsetRotation = FRotator(0, 0, 0);

	// Name of Bone bound to if Position is set to "Attatch to a Bone"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bone Name", Category = "Position"))
	FString boneName = "CC_Base_Head";

	// Choose Directivity Setting for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity", Category = "Directivity"))
	TEnumAsByte<EDir> directivitySetting = none;

	// File Name of the Directivity that sould be used
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name", Category = "Directivity"))
	FString directivityByFileName = "$(DefaultHRIR)";

	// Directivity that is used by a specific phoneme (see Receiver Actor Directivity Manager)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Phoneme Directivity from config", Category = "Directivity"))
	FString directivityByMapping = "";


public:

	// Sets default values for this component's properties
	UVASourceComponent();

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// Plays Sound // 
	UFUNCTION(BlueprintCallable)
	bool PlaySound();

	// Plays Sound from second // 
	UFUNCTION(BlueprintCallable)
	bool PlaySoundFromSecond(float time);

	// Pauses Sound //
	UFUNCTION(BlueprintCallable)
	bool StopSound();

	// Stops Sound //
	UFUNCTION(BlueprintCallable)
	bool PauseSound();

	// returns the Play Sate in form of a EPlayAction // 
	UFUNCTION(BlueprintCallable)
	EPlayAction GetPlayState();

	// Mutes sound // 
	UFUNCTION(BlueprintCallable)
	void MuteSound(bool bMute = true);

	// returns the Position minding its setting for location //
	UFUNCTION(BlueprintCallable)
	FVector GetPosition();

	// returns the Roatation minding its setting for rotation // 
	UFUNCTION(BlueprintCallable)
	FRotator GetRotation();

	// sets the Directivity by the phoneme set up in the ini file
	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByMapping(FString phoneme);

	// sets the Directivity by a file Name
	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByFileName(FString fileName);

	// gets the file name of the current direcitivity 
	UFUNCTION(BlueprintCallable)
	FString GetDirectivityFileName();

	// Sets the visibility of the sound Sound source (used for Debug mode)
	UFUNCTION(BlueprintCallable)
	bool SetSoundSourceVisibility(bool vis_);

	// Preloading sound file for later use
	UFUNCTION(BlueprintCallable)
	bool LoadSoundFile(FString soundFile_);

	// If its a different file than the current, it sets a new Sound for the sound source
	UFUNCTION(BlueprintCallable)
	bool SetSoundFile(FString soundFile_);

	// Returns the file name of the current sound playe back
	UFUNCTION(BlueprintCallable)
	FString GetSoundFile();

	// Returns the file name of the current sound playe back
	UFUNCTION(BlueprintCallable)
	bool setSoundPower(float power);

	// Returns the file name of the current sound playe back
	UFUNCTION(BlueprintCallable)
	float getSoundPower();

	// Sets if the sound source should loop the file
	UFUNCTION(BlueprintCallable)
	bool setLoop(bool loop_);

	// returns whether the sound is looping
	UFUNCTION(BlueprintCallable)
	bool getLoop();


	// Sets the Movement setting (AttatchToBone, ObjectSpawnPoint, MoveWithObject)
	UFUNCTION(BlueprintCallable)
	bool setMovementSetting(EMovement movementSetting_);

	// Sets whether to use Pose Offset
	UFUNCTION(BlueprintCallable)
	bool setUsePoseOffset(bool usePoseOffset_);

	// Changes the Offset for the Position
	UFUNCTION(BlueprintCallable)
	bool setOffsetPosition(FVector pos_);

	// Changes the Offset for the Rotation
	UFUNCTION(BlueprintCallable)
	bool setOffsetRotation(FRotator rot_);

	// Changes the Bone the Sound source is attatched to. If the new bone is found, set MovementSetting to AttatchToBone. Else use old settings
	UFUNCTION(BlueprintCallable)
	bool setBoneName(FString boneName_);

	// Returns the name of the bone the sound is attatched
	UFUNCTION(BlueprintCallable)
	FString getBoneName();


	float getSoundTimeOffset(); // TODO!!
	bool getVisibility(); // TODO!!
	bool getHandleReflections(); // TODO!!

protected:
	// Called when the game starts
	void BeginPlay() override;

	// initialize Sound Source with the settings set // 
	void Initialize();


	// Link to Owner Actor //
	AActor* OwnerActor;

	// The actual VA Sound Source
	VASoundSource* SoundSource = nullptr;

	// skeletal_mesh, used if AttatchToBone is selected
	USkeletalMeshComponent* SkeletalMeshComponent;


	// Check if it is the first tick //
	bool bFirstTick = true;

	// if sound source has initialized //
	bool bInitialized = false;

	// To adapt update rate
	float timeSinceUpdate;


	// Checks if has access to server
	bool shouldSendCommand();


	// Spawn Pos & Rot. Used if ObjectSpawnPoint is selected
	FVector spawnPosition;
	FRotator spawnRotation;


	// Check time // 
	float timer;

	// Check if is Muted
	bool muted = false;


#if WITH_EDITOR
	// Function to improve settings displayed in Editor, can only be used in editor mode
	bool CanEditChange(const UProperty* InProperty) const override;
#endif
};
