#include "VADirectivity.h"

#include "VAPlugin.h"

#include <string.h>


VADirectivity::VADirectivity(FString fileName_) : 
	fileName(fileName_)
{
	dirID = FVAPluginModule::createNewDirectivity(fileName);

}


VADirectivity::VADirectivity(FString fileName_, FString phoneme) : 
	fileName(fileName_)
{
	TArray<FString> tmp;
	tmp.Add(phoneme);
	VADirectivity(fileName, tmp);
}

VADirectivity::VADirectivity(FString fileName_, TArray<FString> phonemes_) : 
	fileName(fileName_), phonemes(phonemes_)
{
	dirID = FVAPluginModule::createNewDirectivity(fileName);
	if (dirID == -1) {
		// FString output = "[VADirectivity::VADirectivity(FString fileName, TArray<FString> phonemes_)] Directivity file " + fileName + " cannot be found!";
		// VAUtils::logStuff(output, true);
		return;
	}
	
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
	return dirID != -1;
}

FString VADirectivity::getFileName()
{
	return fileName;
}
/*
*/