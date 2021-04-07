// Class to represent a Directivity for the VA Server

#pragma once

#include "Containers/UnrealString.h"			// FString
#include "Templates/SharedPointer.h"

class FVAHRIR
{
public:

	// Initialization
	FVAHRIR(FString FileNameN);


	// Getter Functions
	int GetID() const;
	FString GetFileName() const;
	bool IsValidItem() const;

protected:

	void CreateNewHRIR();

	int HRIRID;
	FString FileName;
};


typedef TSharedPtr<FVAHRIR> FVAHRIRSharedPtr;
