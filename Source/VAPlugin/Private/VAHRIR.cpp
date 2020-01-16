#include "VAHRIR.h"

// #include "Containers/UnrealString.h"			// FString
// #include "Containers/Array.h"				// TArray
 
#include "VAPlugin.h"							// For Server
#include "VAHRIRManager.h"				// For default Directivity


VAHRIR::VAHRIR(FString fileName_) :
	fileName(fileName_)
{
	createNewHRIR();
}


void VAHRIR::createNewHRIR()
{
	hrirID = FVAPluginModule::createNewHRIR(fileName);
	if (hrirID == -1) {
		VAUtils::logStuff("[VAHRIR::createNewHRIR()] HRIR file " + fileName + " cannot be found! Using default HRIR instead");
		hrirID = VAHRIRManager::getDefaultHRIR()->getID();
		valid = false;
		return;
	}

	valid = true;
	VAUtils::logStuff("created new VAHRIR");
}

int VAHRIR::getID()
{
	return hrirID;
}


bool VAHRIR::isValid()
{
	return valid;
}

FString VAHRIR::getFileName()
{
	return fileName;
}
