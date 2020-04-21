// Class to represent a Directivity for the VA Server

#pragma once

#include "Containers/UnrealString.h"			// FString

class FVAHRIR
{
public:

	// Initialization
	FVAHRIR(FString FileNameN);


	// Getter Functions
	int GetID() const;
	FString GetFileName() const;
	bool IsValid() const;

protected:

	void CreateNewHRIR();

	int HRIRID;
	FString FileName;
};
