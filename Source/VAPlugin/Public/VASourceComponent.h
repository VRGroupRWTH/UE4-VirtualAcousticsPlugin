// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VAEnums.h"									// EDir, EPlayAction, EMovement
#include "SignalSources/VAAbstractSignalSource.h"
#include "SignalSources/VAAudiofileSignalSource.h"

#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"

#include <string>

#include "VASourceComponent.generated.h"

//forward declarations to not include private header files
class FVASoundSource; //TODO: Remove this
class FVASoundSourceBase;
class FVAImageSourceModel;
class AVAReceiverActor;


UCLASS( ClassGroup=(VA), meta=(BlueprintSpawnableComponent) )
class VAPLUGIN_API UVASourceComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	// Having AVAReceiverActor as friend to be able to call the initialize function from AVAReceiverActor::BeginPlay()
	friend class AVAReceiverActor;



	// Check if reflections by walls should be considered
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Use reflections?", Category = "General Settings"))
	bool bHandleReflections = true;

	// Sound power used for this source
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Sound Power", Category = "General Settings",
		ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "4.0"))
	float SoundPower = 0.0316227749f;


	// Select the class of the signal source
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Signal Type", Category = "Signal Source", AllowAbstract = "false"))
	TSubclassOf<UVAAbstractSignalSource> SignalSourceType = UVAAudiofileSignalSource::StaticClass();

	// Select the type of the signal source
	UPROPERTY(EditAnywhere, Instanced, meta = (DisplayName = "Signal Source", Category = "Signal Source", AllowAbstract = "false"))
	UVAAbstractSignalSource* SignalSource = nullptr;


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
	FString DirectivityByFileName = "$(HumanDir)";

	// Directivity that is used by a specific phoneme (see Receiver Actor Directivity Manager)
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Phoneme Directivity from config", Category = "Directivity"))
	FString DirectivityByMapping = "";


public:

	// Sets default values for this component's properties
	UVASourceComponent();

	void OnComponentCreated() override;

	// Called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// *** Signal Source *** //

	// Sets the signal type used for this sound source
	UFUNCTION(BlueprintCallable)
		bool SetSignalSourceType(TSubclassOf<UVAAbstractSignalSource> SignalSourceTypeN);

	// Gets the signal type used for this sound source
	UFUNCTION(BlueprintCallable)
		TSubclassOf<UVAAbstractSignalSource> GetSignalSourceType() const;

	// Returns a pointer to the signal source component
	UFUNCTION(BlueprintCallable)
		UVAAbstractSignalSource* GetSignalSource() const;

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

	// Sets the Bone name and sets the Movement Setting to follow the bone if it is found. Otherwise keeps old settings
	UFUNCTION(BlueprintCallable)
	bool SetBoneName(FString NewBoneName);

	// Gets the Bone Name
	UFUNCTION(BlueprintCallable)
	FString GetBoneName() const;


protected:
	// Called when the game starts
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// initialize Sound Source with the settings set // 
	void Initialize();

	void UpdatePose();

	bool ForceUpdateSignalSourceType(TSubclassOf<UVAAbstractSignalSource> SignalSourceTypeN);

	bool SetSignalSourceID(const std::string& ID);
	void OnSignalSourceIDChanged(const std::string& ID);

	// *** Event/Delegates *** //

	void BindSignalSourceEvents();
	void UnbindSignalSourceEvents();



	AVAReceiverActor* CurrentReceiverActor;

	TSharedPtr<FVASoundSource> SoundSource; //TODO: Remove this
	TSharedPtr<FVASoundSourceBase> SoundSourceBase;
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

private:
	FDelegateHandle SignalSourceChangedDelegate;

protected:
#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyWhatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Function to improve settings displayed in Editor, can only be used in editor mode
	bool CanEditChange(const FProperty* InProperty) const override;
#endif
	
};
