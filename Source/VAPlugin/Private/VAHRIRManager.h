// Class to manage all the HRIR

#pragma once

#include "VAHRIR.h"

class FVAHRIRManager
{
public:

	// Initialization
	FVAHRIRManager();
	~FVAHRIRManager();

	void ResetManager();

	// Get HRIR
	FVAHRIR* GetHRIRByFileName(FString FileName);

	static FVAHRIR* GetDefaultHRIR();

private:
	TArray<FVAHRIRSharedPtr> HRIRs;

	static FVAHRIR* DefaultHRIR;
};
