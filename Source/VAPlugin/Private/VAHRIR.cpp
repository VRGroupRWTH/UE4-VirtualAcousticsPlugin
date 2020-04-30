#include "VAHRIR.h"

#include "VAPlugin.h"							// For Server
#include "VAUtils.h"			

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
		FVAUtils::LogStuff("[FVAHRIR::createNewHRIR()] HRIR file " + FileName + " cannot be found!");
		return;
	}

	FVAUtils::LogStuff("created a VAHRIR");
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

bool FVAHRIR::IsValidItem() const
{
	return bool(HRIRID != -1);
}

