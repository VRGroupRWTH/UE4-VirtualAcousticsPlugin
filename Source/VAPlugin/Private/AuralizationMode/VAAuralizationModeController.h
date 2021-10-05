// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include <string>
#include <bitset>


#include "VAAuralizationModeController.generated.h"


UENUM(BlueprintType)
namespace EAuralizationMode
{
	enum Type
	{
		DirectSound = 0,
		EarlyReflections = 1,
		DiffuseDecay = 2,
		SourceDirectivity = 3,
		MediumAbsorption = 4,
		TemporalVariation = 5,
		//Scattering = 6,
		//Diffraction = 7,
		//NearField = 8,
		Doppler = 9
		//SpreadingLoss = 10,
		//Transmission = 11,
		//Absorption = 12
	};
}


UCLASS(ClassGroup = (VA), AutoExpandCategories = ("General", "Obstacle Interaction", "Sound Source Related", "Medium Interaction", "Dynamic Effects"),
	meta = (Tooltip = "Only attach to VAReceiverActor, VARenderer (future release) or VASourceComponent"))
class UVAAuralizationModeController : public UObject
{
	GENERATED_BODY()

public:
	// Create a controller for global auralization mode
	UVAAuralizationModeController();
	// Create a controller for renderer auralization mode
	UVAAuralizationModeController(const std::string& RendererName);

	// initialize Sound Source with the settings set // 
	void Initialize();

public:

	// Sets the specified auralization mode to given value (true/false)
	UFUNCTION(BlueprintCallable)
	bool SetAuralizationMode(EAuralizationMode::Type AuralizationMode, bool bEnable);

	// Toggles the specified auralization mode
	UFUNCTION(BlueprintCallable)
	bool ToggleAuralizationMode(EAuralizationMode::Type AuralizationMode);

	// Returns the state of given auralization mode
	UFUNCTION(BlueprintCallable)
	bool GetAuralizationMode(EAuralizationMode::Type AuralizationMode) const;

	//Specifies this auralization mode controller to adjust the modes for renderer of a given name
	void SetTargetRenderer(const std::string& TargetRendererName);

protected:
	const static int NumAuralizationModes = 13;
	typedef std::bitset<NumAuralizationModes> AuralizationBitvector;

	// Sends the given auralization mode bit vector to VA and updates local bitvector on on success
	// @return true on success
	bool SendAuralizationModeToVA(const AuralizationBitvector& AuraModeBitvector);



	// Toggle direct sound
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Direct sound (DS)", Category = "General"))
		bool bEnableDirectSound = true;

	// Toggle early reflections
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Early reflections (ER)", Category = "Obstacle Interaction"))
		bool bEnableEarlyReflections = true;

	// Toggle diffuse decay
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Diffuse decay (DD)", Category = "Obstacle Interaction"))
		bool bEnableDiffuseDecay = true;

	// Toggle source directivity
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Source directivity (SD)", Category = "Sound Source Related"))
		bool bEnableSourceDirectivity = true;

	// Toggle medium absorption
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Medium absorption (MA)", Category = "Medium Interaction"))
		bool bEnableMediumAbsorption = true;

	// Toggle temporal variations
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Temporal variations (TV)", Category = "Medium Interaction"))
		bool bEnableTemporalVariations = true;

	//MISSING: Scattering (SC), Diffraction (DF), Near field (NF)

	// Toggle doppler effect
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Doppler effect (DP)", Category = "Dynamic Effects"))
		bool bEnableDoppler = true;

	//MISSING: Spreading loss (SL), Transmission (TR), Absorption (AB)

	enum ETarget
	{
		Global = 0,
		Renderer = 1
		//SoundSource = 2 //For future
	};
	ETarget Target;

	// Name used to address renderer
	std::string RendererName;

	// Stores the bitvector containing all auralization modes
	AuralizationBitvector AuralizationModeVector;

	bool bInitialized = false;
};
