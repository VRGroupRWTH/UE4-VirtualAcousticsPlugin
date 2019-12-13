#include "VADirectivity.h"

#include "Containers/UnrealString.h"			// FString
#include "Containers/Array.h"					// TArray

#include "VAPlugin.h"							// For Server
#include "VADirectivityManager.h"				// For default Directivity


VADirectivity::VADirectivity(FString fileName_) : 
	fileName(fileName_)
{
	createNewDirectivity();
}


VADirectivity::VADirectivity(FString fileName_, FString phoneme) :
	fileName(fileName_)
{
	TArray<FString> tmp;
	tmp.Add(phoneme);
	phonemes = tmp;

	createNewDirectivity();
}

VADirectivity::VADirectivity(FString fileName_, TArray<FString> phonemes_) : 
	fileName(fileName_), phonemes(phonemes_)
{
	createNewDirectivity();
}

void VADirectivity::createNewDirectivity()
{
	dirID = FVAPluginModule::createNewDirectivity(fileName);
	if (dirID == -1) {
		VAUtils::logStuff("[VADirectivity::VADirectivity()] Directivity file " + fileName + " cannot be found! Using default Directivity instead");
		dirID = VADirectivityManager::getDefaultSourceDirectivity()->getID();
		valid = false;
		return;
	}

	valid = true;
	VAUtils::logStuff("created new VADirectivity");
}

int VADirectivity::getID()
{
	return dirID;
}

void VADirectivity::addPhoneme(FString phoneme)
{
	phonemes.Add(phoneme);
}

void VADirectivity::addPhoneme(TArray<FString> phoneme)
{
	for (auto tmp : phoneme) {
		addPhoneme(tmp);
	}
}

void VADirectivity::logInfo() {
	FString output = " [Info]  Directivity for: ";
	
	if (phonemes.Num() == 0) {
		VAUtils::logStuff("Dir has no phonemes");
	}
	
	for (auto entry : phonemes) {
		output.Append(*entry).Append(",");
	}
	output.RemoveAt(output.Len() - 1, 1, true);
	VAUtils::logStuff(output);
}

bool VADirectivity::containsPhoneme(FString phoneme)
{
	return phonemes.Contains(phoneme);
}

bool VADirectivity::isValid()
{
	return valid;
}

FString VADirectivity::getFileName()
{
	return fileName;
}

/*
*/