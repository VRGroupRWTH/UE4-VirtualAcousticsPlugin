#include "VAHRIR.h"

// #include "Containers/Array.h"				// TArray

#include "VAPlugin.h"							// For Server
#include "VAHRIRManager.h"						// For default Directivity


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVAHRIR::FVAHRIR(const FString FileNameN) :
	FileName(FileNameN)
{
	CreateNewHRIR();
}


void FVAHRIR::CreateNewHRIR()
{
	HRIRID = FVAPlugin::CreateNewHRIR(FileName);
	if (HRIRID == -1)
	{
		FVAUtils::LogStuff("[VAHRIR::createNewHRIR()] HRIR file " + FileName + " cannot be found!");
		return;
	}

	FVAUtils::LogStuff("created new VAHRIR");
}



// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //
// 
int FVAHRIR::GetID() const
{
	return HRIRID;
}

FString FVAHRIR::GetFileName() const
{
	return FileName;
}

bool FVAHRIR::IsValid() const
{
	return bool(HRIRID != -1);
}

