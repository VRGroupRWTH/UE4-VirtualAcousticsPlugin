// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VASoundSource.h"								// From VA

#include "VASourceComponent.generated.h"

// struct used to input Directivity via Variable in UE4 Editor
UENUM()
enum EDir
{
	DefaultHRIR,
	ManualFile,
	Phoneme,
	None
};

UENUM()
enum EMovement
{
	AttachToBone,
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
	FString SoundFile = "WelcomeToVA.wav";

	// Action of the sound source at the first tick
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Action", Category = "General Settings"))
	TEnumAsByte<EPlayAction> StartingPlayAction = Stop;

	// Sets Buffer to a specific time stamp when playing back at the first tick (see Action)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Play from x [s]", Category = "General Settings"))
	float StartingTime = 0.0f;

	// Check if the sound should be played back in a loop
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Loop", Category = "General Settings"))
	bool bLoop = false;

	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use reflections?", Category = "General Settings"))
	bool bHandleReflections = true;

	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Power", Category = "General Settings",
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
	float SoundPower = 0.0316227749f;

	// Decide whether to use manual Transform (below) or use Transform / Movement of Actor
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Position Settings", Category = "Position",
		CustomStructureParam = "Attatch to a Bone, At Object Spawn Point, Move With the Object"))
	TEnumAsByte<EMovement> MovementSetting = ObjectSpawnPoint;

	// Use the manual Offset for the position?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use Offset?", Category = "Position"))
	bool bUsePoseOffset = false;

	// Offset in Position
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset", Category = "Position", EditCondition = "usePoseOffset"))
	FVector OffsetPosition = FVector(0, 0, 0);

	// Offset in Rotation
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Rotation", Category = "Position", EditCondition =
		"usePoseOffset"))
	FRotator OffsetRotation = FRotator(0, 0, 0);

	// Name of Bone bound to if Position is set to "Attatch to a Bone"
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bone Name", Category = "Position"))
	FString BoneName = "CC_Base_Head";

	// Choose Directivity Setting for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity", Category = "Directivity"))
	TEnumAsByte<EDir> DirectivitySetting = None;

	// File Name of the Directivity that should be used
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name", Category = "Directivity"))
	FString DirectivityByFileName = "$(DefaultHRIR)";

	// Directivity that is used by a specific phoneme (see Receiver Actor Directivity Manager)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Phoneme Directivity from config", Category = "Directivity"))
	FString DirectivityByMapping = "";


public:

	// Sets default values for this component's properties
	UVASourceComponent();

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// *** Playback Settings *** //
	
	UFUNCTION(BlueprintCallable)
	bool PlaySound() const;

	UFUNCTION(BlueprintCallable)
	bool PlaySoundFromSecond(float Time) const;

	UFUNCTION(BlueprintCallable)
	bool StopSound() const;

	UFUNCTION(BlueprintCallable)
	bool PauseSound() const;

	UFUNCTION(BlueprintCallable)
	EPlayAction GetPlayState() const;


	// *** Sound Settings *** // 

	UFUNCTION(BlueprintCallable)
	bool MuteSound(bool bMuteN = true);

	UFUNCTION(BlueprintCallable)
	bool LoadSoundFile(FString SoundFileN) const;

	UFUNCTION(BlueprintCallable)
	bool SetSoundFile(FString SoundFileN);

	UFUNCTION(BlueprintCallable)
	FString GetSoundFile() const;

	UFUNCTION(BlueprintCallable)
	bool SetSoundPower(float Power);

	UFUNCTION(BlueprintCallable)
	float GetSoundPower() const;

	UFUNCTION(BlueprintCallable)
	bool SetLoop(bool bLoopN);

	UFUNCTION(BlueprintCallable)
	bool GetLoop() const;
	
	UFUNCTION(BlueprintCallable)
	bool GetHandleReflections() const;

	
	// *** Sound Pose *** //
	
	UFUNCTION(BlueprintCallable)
	FVector GetPosition() const;

	UFUNCTION(BlueprintCallable)
	FRotator GetRotation() const;

	UFUNCTION(BlueprintCallable)
	bool SetMovementSetting(EMovement MovementSettingN);

	UFUNCTION(BlueprintCallable)
	bool SetUsePoseOffset(bool bUsePoseOffsetN);

	UFUNCTION(BlueprintCallable)
	bool SetOffsetPosition(FVector PosN);

	UFUNCTION(BlueprintCallable)
	bool SetOffsetRotation(FRotator RotN);


	// *** DIRECTIVITIES *** // 
	
	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByMapping(FString Phoneme);

	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByFileName(FString FileName);

	UFUNCTION(BlueprintCallable)
	FString GetDirectivityFileName() const;


	// *** GRAPHICAL REPRESENTATION *** // 
	
	UFUNCTION(BlueprintCallable)
	bool SetVisibility(bool bVisN) const;

	UFUNCTION(BlueprintCallable)
	bool GetVisibility() const;
	
	UFUNCTION(BlueprintCallable)
	bool SetBoneName(FString BoneNameN);

	UFUNCTION(BlueprintCallable)
	FString GetBoneName() const;




	float GetSoundTimeOffset() const;

protected:
	// Called when the game starts
	void BeginPlay() override;

	// initialize Sound Source with the settings set // 
	void Initialize();

	FVASoundSource* SoundSource;
	USkeletalMeshComponent* SkeletalMeshComponent;

	// Class data
	bool bFirstTick = true;
	bool bInitialized = false;
	float TimeSinceUpdate;
	bool ShouldSendCommand() const;
	float Timer;

	
	// Sound Source data
	FVector SpawnPosition;
	FRotator SpawnRotation;
	bool bMuted = false;
	int UpdateRate;

#if WITH_EDITOR
	// Function to improve settings displayed in Editor, can only be used in editor mode
	bool CanEditChange(const UProperty* InProperty) const override;
#endif
	
};
