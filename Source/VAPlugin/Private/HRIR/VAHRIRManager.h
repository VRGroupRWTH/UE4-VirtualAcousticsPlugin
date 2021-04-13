// Class to manage all the HRIR

#pragma once

#include "HRIR/VAHRIR.h"

#include <Templates/SharedPointer.h>

class FVAHRIRManager
{
public:

	// Initialization
	FVAHRIRManager();
	~FVAHRIRManager();

	void ResetManager();

	// Get HRIR
	TSharedPtr<FVAHRIR> GetHRIRByFileName(FString FileName);

	static TSharedPtr<FVAHRIR> GetDefaultHRIR();

private:
	TArray<FVAHRIRSharedPtr> HRIRs;

	static TSharedPtr<FVAHRIR> DefaultHRIR;
};
