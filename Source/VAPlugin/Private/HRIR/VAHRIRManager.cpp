#include "HRIR/VAHRIRManager.h"

#include "VAUtils.h"

TSharedPtr<FVAHRIR> FVAHRIRManager::DefaultHRIR;



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
	DefaultHRIR.Reset();
	DefaultHRIR = MakeShared<FVAHRIR>("$(DefaultHRIR)");

	HRIRs.Empty();
	HRIRs.Add(DefaultHRIR);
}

// ****************************************************************** // 
// ******* Get HRIR ************************************************* //
// ****************************************************************** //

TSharedPtr<FVAHRIR> FVAHRIRManager::GetHRIRByFileName(const FString FileName)
{
	// Search for already existing HRIR
	if (HRIRs.Num() > 0)
	{
 		for (auto Entry : HRIRs)
 		{
 			if (Entry.Get()->GetFileName() == FileName)
 			{
 				FVAUtils::LogStuff("[FVAHRIRManager::GetHRIRByFileName()]: HRIR from file " + 
					FileName + " was found!", false);
 
 				return Entry;
 			}
 		}
	}

	FVAUtils::LogStuff(	"[FVAHRIRManager::GetHRIRByFileName()]: HRIR from file " + 
		FileName + " cannot be found! Creating one now...", false);

	FVAHRIRSharedPtr NewHRIR(new FVAHRIR(FString(FileName)));
	
	if (NewHRIR.IsValid() && NewHRIR->IsValidItem())
	{
		FVAUtils::LogStuff("[FVAHRIRManager::GetHRIRByFileName()]: HRIR from file " + 
			FileName + " is created!", false);
		HRIRs.Add(NewHRIR);
		return NewHRIR;
	}
	FVAUtils::LogStuff("[FVAHRIRManager::GetHRIRByFileName()]: HRIR from file " + 
		FileName + " cannot be created!", true);

	// Return Default HRIR
	return GetDefaultHRIR();
}

TSharedPtr<FVAHRIR> FVAHRIRManager::GetDefaultHRIR()
{
	if (DefaultHRIR.IsValid())
	{
		return DefaultHRIR;
	}
	FVAUtils::LogStuff("[FVAHRIRManager::GetDefaultHRIR()]: No default HRIR found. Please have a scene loaded " +
		FString("so a ReceiverActor exists handling a HRIRManager containing the defaultHRIR."), true);
	return nullptr;
}


