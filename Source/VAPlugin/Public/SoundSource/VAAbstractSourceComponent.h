// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAEnums.h"									// EDir, EPlayAction, EMovement

#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"

#include <string>

#include "VAAbstractSourceComponent.generated.h"

//forward declarations to not include private header files
class FVASoundSource;
class FVAImageSourceModel;
class AVAReceiverActor;
class UVAAbstractSignalSource;


UCLASS( ClassGroup=(VA), Abstract, meta=(BlueprintSpawnableComponent), HideDropdown )
class VAPLUGIN_API UVAAbstractSourceComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	// Having AVAReceiverActor as friend to be able to call the initialize function from AVAReceiverActor::BeginPlay()
	friend class AVAReceiverActor;



	// Sound power used for this source
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Power", Category = "General Settings",
		ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
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
	FString BoneName = "head";

	// Choose Directivity Setting for Receiver
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity", Category = "Directivity"))
	TEnumAsByte<EDirectivitySetting::Type> DirectivitySetting = EDirectivitySetting::Type::NoDirectivity;

	// File Name of the Directivity that should be used
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Directivity by file name", Category = "Directivity"))
	FString DirectivityByFileName = "$(HumanDir)";

	// Directivity that is used by a specific phoneme (see Receiver Actor Directivity Manager)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Phoneme Directivity from config", Category = "Directivity"))
	FString DirectivityByMapping = "";

	// Activate to generate 1st order image sources based on VAReflectionWalls
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Enable", Category = "Image Source Model"))
	bool bHandleReflections = false;


public:

	// Sets default values for this component's properties
	UVAAbstractSourceComponent();

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// *** Sound Source Settings *** //

	// Mute sound (true = mute)
	UFUNCTION(BlueprintCallable)
	bool MuteSound(bool bMute = true);

	// Sets the Sound Power in Watts
	UFUNCTION(BlueprintCallable)
	bool SetSoundPower(float Power);

	// Gets the Sound Power in Watts
	UFUNCTION(BlueprintCallable)
	float GetSoundPower() const;

	// *** Image Sources *** //

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
	bool SetMovementSetting(EMovement::Type NewMovementSetting);

	// Sets the Bone name and sets the Movement Setting to follow the bone if it is found. Otherwise keeps old settings
	UFUNCTION(BlueprintCallable)
	bool SetBoneName(FString NewBoneName);

	// Gets the Bone Name
	UFUNCTION(BlueprintCallable)
	FString GetBoneName() const;

	// Sets to use Position Offset
	UFUNCTION(BlueprintCallable)
	bool SetUsePoseOffset(bool bNewUsePoseOffset);

	// Sets the Position Offset. If Offset disabled it gets enabled
	UFUNCTION(BlueprintCallable)
	bool SetOffsetPosition(FVector Pos);

	// Sets the Rotation Offset. If Offset disabled it gets enabled
	UFUNCTION(BlueprintCallable)
	bool SetOffsetRotation(FRotator Rot);


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
	virtual bool SetVisibility(bool bVisibility);

	// Gets the Visibility of the Sound Source and its reflections.
	UFUNCTION(BlueprintCallable)
	bool GetVisibility() const;


protected:

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// initialize Sound Source with the settings set // 
	virtual void Initialize();

	void UpdatePose();

	bool SetSignalSourceID(const std::string& ID);

	AVAReceiverActor* CurrentReceiverActor;

	TSharedPtr<FVASoundSource> SoundSource;
	TSharedPtr<FVAImageSourceModel> ImageSourceModel;
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
	int UpdateRate;

	UPROPERTY(EditAnywhere, Instanced, SimpleDisplay, NoClear, meta = (Category = "Signal Source (what to play?)"))
	UVAAbstractSignalSource* SignalSource;

#if WITH_EDITOR
	// Function to improve settings displayed in Editor, can only be used in editor mode
	bool CanEditChange(const FProperty* InProperty) const override;
#endif
	
};