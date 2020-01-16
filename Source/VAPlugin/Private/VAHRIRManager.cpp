#include "VAHRIRManager.h"

#include "VAUtils.h"

#include "Core.h"
#include "Interfaces/IPluginManager.h"

VAHRIR* VAHRIRManager::defaultHRIR = nullptr;

VAHRIRManager::VAHRIRManager()
{
}

VAHRIRManager::~VAHRIRManager()
{
	defaultHRIR = nullptr;
}



void VAHRIRManager::reset()
{
	// TODO: delete all HRIR from the server

	if (defaultHRIR != nullptr) {
		delete defaultHRIR;
	}
	defaultHRIR = new VAHRIR(FString("$(DefaultHRIR)"));

	hrirs.Empty();
	hrirs.Add(defaultHRIR);
}


VAHRIR* VAHRIRManager::getHRIRByFileName(FString fileName_)
{
	// Find already existing HRIR
	if (!hrirs.Num()) {
		for (auto entry : hrirs) {
			if (entry->getFileName() == fileName_) {
				FString output = "[VAHRIRManager::getHRIRByFileName()] HRIR from file " + fileName_ + " was found!";
				VAUtils::logStuff(output);

				return entry;
			}
		}
	}
	
	
	VAUtils::logStuff("[VAHRIRManager::getHRIRByFileName()] HRIR from file " + fileName_ + " cannot be found! Creating one now...");

	// Create a new HRIR
	VAHRIR* new_hrir = new VAHRIR(fileName_);
	if (new_hrir != nullptr) {
		FString output = "[VAHRIRManager::getHRIRByFileName()] HRIR from file " + fileName_ + " is created!";
		VAUtils::logStuff(output);
		hrirs.Add(new_hrir);
		return new_hrir;
	}
	else {
		VAUtils::logStuff("[VAHRIRManager::getHRIRByFileName()] HRIR from file " + fileName_ + " cannot be created!");
	}

	// Return Default HRIR
	return defaultHRIR;
}



VAHRIR * VAHRIRManager::getDefaultHRIR()
{
	if (defaultHRIR == nullptr) {
		defaultHRIR = new VAHRIR(FString("$(DefaultHRIR)"));
	}

	return defaultHRIR;
}


