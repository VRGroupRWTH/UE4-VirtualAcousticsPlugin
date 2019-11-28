#include "VADirectivity.h"

#include "VAPlugin.h"


VADirectivity::VADirectivity() 
	: dirID(-1)
{

}

VADirectivity::VADirectivity(int dirID_, TArray<FString> phonemes_)
	: dirID(dirID_), phonemes(phonemes_)
{

}

VADirectivity::VADirectivity(FString fileName, FString phoneme)
{
	TArray<FString> tmp;
	tmp.Add(phoneme);
	VADirectivity(fileName, tmp);
}

VADirectivity::VADirectivity(FString fileName, TArray<FString> phonemes_)
	: phonemes(phonemes_)
{
	dirID = FVAPluginModule::createNewDirectivity(fileName);
	if (dirID == -1) {
		UE_LOG(LogTemp, Warning, TEXT("   Directivity file %s cannot be found!"), *fileName);
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

void VADirectivity::logInfo() {
	FString output = "";
	for (auto entry : phonemes) {
		output.Append(*entry).Append(",");
	}
	output.RemoveAt(output.Len() - 1, 1, true);
	UE_LOG(LogTemp, Warning, TEXT(" [Info]  Directivity for: "), *output);
}

bool VADirectivity::containsPhoneme(FString phoneme)
{
	return phonemes.Contains(phoneme);
}

bool VADirectivity::isValid()
{
	return dirID != -1;
}
/*
*/