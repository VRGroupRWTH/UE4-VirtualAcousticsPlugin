#include "VADirectivityManager.h"

#include "VAUtils.h"

#include "Core.h"
#include "Interfaces/IPluginManager.h"

VADirectivity* VADirectivityManager::defaultReceiverDirectivity = nullptr;
VADirectivity* VADirectivityManager::defaultSourceDirectivity = nullptr;

VADirectivityManager::VADirectivityManager()
	: configFileName("")
{

}

VADirectivityManager::VADirectivityManager(FString configFileName_)
{
	readConfigFile(configFileName);
}


void VADirectivityManager::readConfigFile(FString configFileName_) {

	/*
	*/
	directivities.Empty();
	configFileName = configFileName_;

	defaultReceiverDirectivity = new VADirectivity(FString("$(DefaultHRIR)"));
	defaultSourceDirectivity   = new VADirectivity(FString("$(HumanDir)"));

	directivities.Add(defaultReceiverDirectivity);
	directivities.Add(defaultSourceDirectivity);


	// Read config File
	// FString BaseDir = IPluginManager::Get().FindPlugin("VAPlugin")->GetBaseDir();
	// FString dir = FPaths::Combine(*BaseDir, TEXT("config/directivities/"));

	FString RelativePath = FPaths::ProjectContentDir();
	FString dir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath) + "Study/";

	FString config_file_name = dir + configFileName;

	GConfig->UnloadFile(config_file_name);
	GConfig->LoadFile(config_file_name);

	FConfigFile* config = GConfig->FindConfigFile(config_file_name);

	if (config == nullptr) {
		VAUtils::logStuff(FString("[VADirectivityManager::readConfigFile()] - Unable to load directivity config file: ") + config_file_name);
		return;
	}
	VAUtils::logStuff(FString("[VADirectivityManager::readConfigFile()] - Config file loaded: ") + config_file_name);


	FString listSymbol, mapToSymbol;
	config->GetString(TEXT("DirectivityMapping"), TEXT("listSymbol"), listSymbol);
	config->GetString(TEXT("DirectivityMapping"), TEXT("mapToSymbol"), mapToSymbol);

	TArray<FString> mapping;
	config->GetArray(TEXT("DirectivityMapping"), TEXT("mapping"), mapping);


	// Format a,b,c:file.data
	TArray<FString> tmp_phonemes;
	FString tmp,
		tmp_list,		// for tmp of saving the list of the phonemes "a,b,c"
		tmp_fileName,	// for tmp of savint the file name "file.data"
		tmp_phoneme;	// for saving each phoneme to bring tmp_list to TArray "a","b","c"


	for (auto entry : mapping) {
		if (!entry.Contains(mapToSymbol)) {
			UE_LOG(LogTemp, Warning, TEXT("   Directivity %s has a wrong format! (no mapToSymbol)"), *entry);
			continue;
		}

		tmp = *entry;
		// Separate for Phoneme List and File Name
		tmp.Split(mapToSymbol, &tmp_list, &tmp_fileName);

		if (tmp_list.IsEmpty() || tmp_fileName.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("   Directivity %s has a wrong format! (no list of phonemes or file name)"), *entry);
			continue;
		}

		// Convert tmp_list into TArray of all phonemes
		tmp_phonemes.Empty();
		while (tmp_list.Contains(listSymbol)) {
			tmp_list.Split(listSymbol, &tmp_phoneme, &tmp_list);
			tmp_phonemes.Add(tmp_phoneme);
		}
		tmp_phonemes.Add(tmp_list);



		// int dirID = FVAPluginModule::createNewDirectivity(tmp_fileName);
		// if (dirID == -1) {
		// 	UE_LOG(LogTemp, Warning, TEXT("   Directivity file %s cannot be found!"), tmp_fileName);
		// 	continue;
		// }

		FString output = "Mapping from: ";
		for (auto p : tmp_phonemes) {
			output.Append(p + ",");
		}
		output.Append(" --> " + tmp_fileName);
		VAUtils::logStuff(output);

		VADirectivity* tmpDir = getDirectivityByFileName(tmp_fileName);

		// If there is no Dir from that file
		if (tmpDir == nullptr) {
			tmpDir = new VADirectivity(tmp_fileName, tmp_phonemes);
			directivities.Add(tmpDir);
		}
		else { // If there already is an existing Dir with that file
			tmpDir->addPhoneme(tmp_phonemes);
		}

		// If it is the default directivity
		if (tmp_phonemes.Contains("default")) {
			defaultDir = tmpDir;
		}

	}



}

void VADirectivityManager::reset()
{
	defaultReceiverDirectivity 	 = nullptr;
	defaultSourceDirectivity 	 = nullptr;
	defaultDir					 = nullptr;
	
	directivities.Empty();
}

VADirectivity* VADirectivityManager::getDirectivityByPhoneme(FString phoneme)
{
	for (auto entry : directivities) {
		if (entry->containsPhoneme(phoneme)) {
			return entry;
		}
	}
	FString output = "[VADirectivityManager::getDirectivityByPhoneme()] Directivity for phoneme " + phoneme + " cannot be found!";
	VAUtils::logStuff(output);

	return defaultDir;
}

VADirectivity* VADirectivityManager::getDirectivityByFileName(FString fileName_)
{
	for (auto entry : directivities) {
		if (entry->getFileName() == fileName_) {
			FString output = "[VADirectivityManager::getDirectivityByFileName()] Directivity from file " + fileName_ + " was found!";
			VAUtils::logStuff(output);

			return entry;
		}
	}
	FString output = "[VADirectivityManager::getDirectivityByFileName()] Directivity from file " + fileName_ + " cannot be found!";
	VAUtils::logStuff(output);

	return defaultDir;
}

bool VADirectivityManager::isValid()
{
	return configFileName != "";
}

void VADirectivityManager::printDirMapping()
{
	for (auto entry : directivities) {
		entry->logInfo();
	}
}

VADirectivity * VADirectivityManager::getDefaultReceiverDirectivity()
{
	if (defaultReceiverDirectivity == nullptr) {
		defaultReceiverDirectivity = new VADirectivity(FString("$(DefaultHRIR)"));
	}

	return defaultReceiverDirectivity;
}

VADirectivity * VADirectivityManager::getDefaultSourceDirectivity()
{
	if (defaultSourceDirectivity == nullptr) {
		defaultSourceDirectivity = new VADirectivity(FString("$(HumanDir)"));
	}

	return defaultSourceDirectivity;
}

