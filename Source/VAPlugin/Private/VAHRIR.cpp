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
		VAUtils::logStuff("[VAHRIR::createNewHRIR()] HRIR file " + fileName + " cannot be found!");
		return;
	}

	VAUtils::logStuff("created new VAHRIR");
}

int VAHRIR::getID()
{
	return hrirID;
}


bool VAHRIR::isValid()
{
	return (hrirID != -1);
}

FString VAHRIR::getFileName()
{
	return fileName;
}
