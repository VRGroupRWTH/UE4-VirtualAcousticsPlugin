// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAEnums.h"									// EDir, EPlayAction, EMovement

#include "GameFramework/Actor.h"
#include "VASoundSource.h"								// From VA
#include "SharedPointer.h"

#include "VASourceComponent.generated.h"


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
	TEnumAsByte<EPlayAction::Type> StartingPlayAction = EPlayAction::Type::Stop;

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
		CustomStructureParam = "Move With the Object, At Object Spawn Point (unmovable, also reflections), Attatch to a Bone"))
	TEnumAsByte<EMovement::Type> MovementSetting = EMovement::Type::MoveWithObject;

	// Use the manual Offset for the position?
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use Offset?", Category = "Position"))
	bool bUsePoseOffset = false;

	// Offset in Position
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Offset Position", Category = "Position", EditCondition = "usePoseOffset"))
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
	TEnumAsByte<EDirectivitySetting::Type> DirectivitySetting = EDirectivitySetting::Type::NoDirectivity;

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

	// Play Sound
	UFUNCTION(BlueprintCallable)
	bool PlaySound();

	// Play Sound starting from second of sound
	UFUNCTION(BlueprintCallable)
	bool PlaySoundFromSecond(float Time);

	// Stop Sound
	UFUNCTION(BlueprintCallable)
	bool StopSound();

	// Pause Sound
	UFUNCTION(BlueprintCallable)
	bool PauseSound();

	// Returns the Play State directly from the server 	NoPlayAction = -1, Stop = 0, Pause = 1,	Play = 2
	UFUNCTION(BlueprintCallable)
	EPlayAction::Type GetPlayState() const;


	// *** Sound Settings *** // 

	// Mute sound (true = mute)
	UFUNCTION(BlueprintCallable)
	bool MuteSound(bool bMuteN = true);

	// (Pre-) loads a sound file for later usage
	UFUNCTION(BlueprintCallable)
	bool LoadSoundFile(FString SoundFileN);

	// Sets a sound file. If already loaded use the loaded one, if not create new
	UFUNCTION(BlueprintCallable)
	bool SetSoundFile(FString SoundFileN);

	// Returns the name of the sound file set at the moment
	UFUNCTION(BlueprintCallable)
	FString GetSoundFile() const;

	// Sets the Sound Power in Watts
	UFUNCTION(BlueprintCallable)
	bool SetSoundPower(float Power);

	// Gets the Sound Power in Watts
	UFUNCTION(BlueprintCallable)
	float GetSoundPower() const;

	// Sets whether to loop or not (true = loop)
	UFUNCTION(BlueprintCallable)
	bool SetLoop(bool bLoopN);

	// Gets whether to loop or not (true = loop)
	UFUNCTION(BlueprintCallable)
	bool GetLoop() const;

	// Gets whether to handle the reflections on this sound source
	UFUNCTION(BlueprintCallable)
	bool GetHandleReflections() const;

	
	// *** Sound Pose *** //

	// Gets Position of Sound Source
	UFUNCTION(BlueprintCallable)
	FVector GetPosition() const;

	// Gets Rotation of Sound Source
	UFUNCTION(BlueprintCallable)
	FRotator GetRotation() const;

	// Sets Movement Setting 	MoveWithObject = 0, ObjectSpawnPoint = 1, AttachToBone = 2,
	UFUNCTION(BlueprintCallable)
	bool SetMovementSetting(EMovement::Type MovementSettingN);

	// Sets to use Position Offset
	UFUNCTION(BlueprintCallable)
	bool SetUsePoseOffset(bool bUsePoseOffsetN);

	// Sets the Position Offset. If Offset disabled it gets enabled
	UFUNCTION(BlueprintCallable)
	bool SetOffsetPosition(FVector PosN);

	// Sets the Rotation Offset. If Offset disabled it gets enabled
	UFUNCTION(BlueprintCallable)
	bool SetOffsetRotation(FRotator RotN);


	// *** Directivities *** //
	
	// Sets Directivity by searching for its corresponding Directivity from the Mapping 
	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByMapping(FString Phoneme);

	// Sets Directivity by a File Name, Passing a empty string "" removes the directivity, passing "default" sets the default directivity
	UFUNCTION(BlueprintCallable)
	bool SetDirectivityByFileName(FString FileName);
	
	// Gets the File Name of the Directivity
	UFUNCTION(BlueprintCallable)
	FString GetDirectivityFileName() const;


	// *** Graphical Representation *** // 

	// Sets the Visibility of the Sound Source and its reflections.
	UFUNCTION(BlueprintCallable)
	bool SetVisibility(bool bVisN);

	// Gets the Visibility of the Sound Source and its reflections.
	UFUNCTION(BlueprintCallable)
	bool GetVisibility() const;

	// Sets the Bone name and sets the Movement Setting to follow the bone if it is found. Otherwise keeps old settings
	UFUNCTION(BlueprintCallable)
	bool SetBoneName(FString BoneNameN);

	// Gets the Bone Name
	UFUNCTION(BlueprintCallable)
	FString GetBoneName() const;




	float GetSoundTimeOffset() const;

protected:
	// Called when the game starts
	void BeginPlay() override;

	// initialize Sound Source with the settings set // 
	void Initialize();

	AVAReceiverActor* CurrentReceiverActor;

	TSharedPtr<FVASoundSource> SoundSource;
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
