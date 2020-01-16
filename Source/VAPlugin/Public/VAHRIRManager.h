// Class to manage all the HRIR

#pragma once

#include "VAHRIR.h"

class VAHRIRManager
{
public:
	VAHRIRManager();
	~VAHRIRManager();

	void reset();

	VAHRIR* getHRIRByFileName(FString fileName_);
	
	static VAHRIR* getDefaultHRIR();

private:
	TArray<VAHRIR*> hrirs;

	static VAHRIR* defaultHRIR;

};