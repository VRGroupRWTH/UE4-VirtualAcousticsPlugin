#include "VAHRIRManager.h"

#include "VAUtils.h"

FVAHRIR* FVAHRIRManager::DefaultHRIR;



// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVAHRIRManager::FVAHRIRManager()
{
}

FVAHRIRManager::~FVAHRIRManager()
{
	HRIRs.Reset();
	DefaultHRIR = nullptr;
}


void FVAHRIRManager::ResetManager()
{
	DefaultHRIR = nullptr;
	// TODO new
	FVAHRIRSharedPtr NewHRIR(new FVAHRIR("$(DefaultHRIR)"));
	DefaultHRIR = NewHRIR.Get();

	HRIRs.Empty();
	HRIRs.Add(NewHRIR);
}

// ****************************************************************** // 
// ******* Get HRIR ************************************************* //
// ****************************************************************** //

FVAHRIR* FVAHRIRManager::GetHRIRByFileName(const FString FileName)
{
	// Search for already existing HRIR
	if (HRIRs.Num() > 0)
	{
 		for (auto Entry : HRIRs)
 		{
 			if (Entry.Get()->GetFileName() == FileName)
 			{
 				FVAUtils::LogStuff("[FVAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + 
					FileName + " was found!");
 
 				return Entry.Get();
 			}
 		}
	}

	FVAUtils::LogStuff(
		"[FVAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + FileName + 
		" cannot be found! Creating one now...");

	// TODO new
	FVAHRIRSharedPtr NewHRIR(new FVAHRIR(FString(FileName)));
	
	if (NewHRIR.IsValid() && NewHRIR->IsValidItem())
	{
		FVAUtils::LogStuff("[FVAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + 
			FileName + " is created!");
		HRIRs.Add(NewHRIR);
		return NewHRIR.Get();
	}
	FVAUtils::LogStuff("[FVAHeadRelatedIRManager::getHRIRByFileName()] HRIR from file " + 
		FileName + " cannot be created!");

	// Return Default HRIR
	return GetDefaultHRIR();
}

FVAHRIR* FVAHRIRManager::GetDefaultHRIR()
{
	if (DefaultHRIR != nullptr)
	{
		return DefaultHRIR;
	}
	FVAUtils::LogStuff("[FVAHRIRManager::GetDefaultHRIR()] No default HRIR found. Please have a scene loaded " +
		FString("so a ReceiverActor exists handling a HRIRManager containing the defaultHRIR."));
	return nullptr;
}


