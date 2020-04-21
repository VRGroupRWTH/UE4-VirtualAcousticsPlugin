#include "VAHRIRManager.h"

#include "FVAUtils.h"

FVAHRIR* FVAHRIRManager::DefaultHRIR = nullptr;



// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVAHRIRManager::FVAHRIRManager()
{
}

FVAHRIRManager::~FVAHRIRManager()
{
	DefaultHRIR = nullptr;
}


void FVAHRIRManager::ResetManager()
{
	if (DefaultHRIR != nullptr)
	{
		delete DefaultHRIR;
	}
	DefaultHRIR = new FVAHRIR(FString("$(DefaultHRIR)"));

	HRIRs.Empty();
	HRIRs.Add(DefaultHRIR);
}

// ****************************************************************** // 
// ******* Get HRIR ************************************************* //
// ****************************************************************** //

FVAHRIR* FVAHRIRManager::GetHRIRByFileName(const FString FileName)
{
	// Find already existing HRIR
	if (!HRIRs.Num())
	{
		for (auto Entry : HRIRs)
		{
			if (Entry->GetFileName() == FileName)
			{
				FVAUtils::LogStuff("[VAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + FileName + " was found!");

				return Entry;
			}
		}
	}


	FVAUtils::LogStuff(
		"[VAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + FileName + " cannot be found! Creating one now...");

	// Create a new HRIR
	FVAHRIR* NewHRIR = new FVAHRIR(FileName);
	if (NewHRIR != nullptr)
	{
		FVAUtils::LogStuff("[VAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + FileName + " is created!");
		HRIRs.Add(NewHRIR);
		return NewHRIR;
	}
	FVAUtils::LogStuff("[VAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + FileName + " cannot be created!");

	// Return Default HRIR
	return DefaultHRIR;
}


FVAHRIR* FVAHRIRManager::GetDefaultHRIR()
{
	if (DefaultHRIR == nullptr)
	{
		DefaultHRIR = new FVAHRIR(FString("$(DefaultHRIR)"));
	}

	return DefaultHRIR;
}
