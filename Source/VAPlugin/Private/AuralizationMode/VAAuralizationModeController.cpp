// Fill out your copyright notice in the Description page of Project Settings.


#include "AuralizationMode/VAAuralizationModeController.h"


#include "VAUtils.h"
#include "VAPlugin.h"

// Sets default values for this component's properties
UVAAuralizationModeController::UVAAuralizationModeController()
	: Target(Global)
{
}


UVAAuralizationModeController::UVAAuralizationModeController(const std::string& RendererName)
	: Target(Renderer)
	, RendererName(RendererName)
{
}


void UVAAuralizationModeController::Initialize()
{
	if (bInitialized)
	{
		return;
	}

	AuralizationModeVector.set();
	AuralizationModeVector[(int)EAuralizationMode::DirectSound] = bEnableDirectSound;
	AuralizationModeVector[(int)EAuralizationMode::EarlyReflections] = bEnableEarlyReflections;
	AuralizationModeVector[(int)EAuralizationMode::DiffuseDecay] = bEnableDiffuseDecay;
	AuralizationModeVector[(int)EAuralizationMode::SourceDirectivity] = bEnableSourceDirectivity;
	AuralizationModeVector[(int)EAuralizationMode::MediumAbsorption] = bEnableMediumAbsorption;
	AuralizationModeVector[(int)EAuralizationMode::TemporalVariation] = bEnableTemporalVariations;
	AuralizationModeVector[(int)EAuralizationMode::Doppler] = bEnableDoppler;
	SendAuralizationModeToVA(AuralizationModeVector);
	bInitialized = true;
}


bool UVAAuralizationModeController::SetAuralizationMode(EAuralizationMode::Type AuralizationMode, bool bEnable)
{
	std::bitset<NumAuralizationModes> TmpAuraModeBitvector = AuralizationModeVector;
	TmpAuraModeBitvector[int(AuralizationMode)] = bEnable;

	return SendAuralizationModeToVA(TmpAuraModeBitvector);
}

bool UVAAuralizationModeController::ToggleAuralizationMode(EAuralizationMode::Type AuralizationMode)
{
	std::bitset<NumAuralizationModes> TmpAuraModeBitvector = AuralizationModeVector;
	TmpAuraModeBitvector.flip(int(AuralizationMode));

	return SendAuralizationModeToVA(TmpAuraModeBitvector);
}

bool UVAAuralizationModeController::GetAuralizationMode(EAuralizationMode::Type AuralizationMode) const
{
	return AuralizationModeVector[AuralizationMode];
}

void UVAAuralizationModeController::SetTargetRenderer(const std::string& TargetRendererName)
{
	RendererName = TargetRendererName;
	Target = Renderer;
}

bool UVAAuralizationModeController::SendAuralizationModeToVA(const AuralizationBitvector& AuraModeBitvector)
{
	bool ok = false;
	switch (Target)
	{
	case Global:
		ok = FVAPlugin::SetGlobalAuralizationMode((int)AuraModeBitvector.to_ulong());
		break;
	case Renderer:
		ok = FVAPlugin::SetRenderingModuleAuralizationMode(RendererName, (int)AuraModeBitvector.to_ulong());
		break;
	}
	if (ok)
	{
		AuralizationModeVector = AuraModeBitvector;
	}
	return ok;
}

