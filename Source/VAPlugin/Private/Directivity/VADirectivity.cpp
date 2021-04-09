#include "Directivity/VADirectivity.h"

#include "VAPlugin.h"							// For Server Interface functions
#include "VAUtils.h"

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVADirectivity::FVADirectivity(const FString FileNameN) :
	FileName(FileNameN)
{
	CreateNewDirectivity();
}

FVADirectivity::FVADirectivity(const FString FileNameN, const FString Phoneme) :
	FileName(FileNameN)
{
	TArray<FString> Tmp;
	Tmp.Add(Phoneme);
	Phonemes = Tmp;

	CreateNewDirectivity();
}

FVADirectivity::FVADirectivity(const FString FileNameN, const TArray<FString> PhonemesN) :
	FileName(FileNameN), Phonemes(PhonemesN)
{
	CreateNewDirectivity();
}

void FVADirectivity::CreateNewDirectivity()
{
	DirID = FVAPlugin::CreateNewDirectivity(FileName);
	if (DirID == -1)
	{
		FVAUtils::LogStuff("[FVADirectivity::createNewDirectivity()]: Directivity file " + 
			FileName + " cannot be created!", true);
		return;
	}

	FVAUtils::LogStuff("[FVADirectivity::createNewDirectivity()]: Created a new VADirectivity", false);
}


// ****************************************************************** // 
// ******* Phoneme handling ***************************************** //
// ****************************************************************** //

void FVADirectivity::AddPhoneme(const FString Phoneme)
{
	Phonemes.Add(Phoneme);
}

void FVADirectivity::AddPhoneme(const TArray<FString> PhonemesN)
{
	for (const FString& Phoneme : PhonemesN)
	{
		AddPhoneme(Phoneme);
	}
}


// ****************************************************************** // 
// ******* Check Phonemes ******************************************* //
// ****************************************************************** //

bool FVADirectivity::ContainsPhoneme(const FString Phoneme) const
{
	return Phonemes.Contains(Phoneme);
}

bool FVADirectivity::IsValidItem() const
{
	return (DirID != -1);
}

void FVADirectivity::LogInfo() const
{
	FString Output = " [FVADirectivity::LogInfo()]: Directivity for: ";

	if (Phonemes.Num() == 0)
	{
		FVAUtils::LogStuff("[FVADirectivity::LogInfo()]: Directivity has no phonemes", false);
	}

	for (auto Entry : Phonemes)
	{
		Output.Append(*Entry).Append(",");
	}

	Output.RemoveAt(Output.Len() - 1, 1, true);

	FVAUtils::LogStuff(Output, false);
}

// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

int FVADirectivity::GetID() const
{
	return DirID;
}

FString FVADirectivity::GetFileName() const
{
	return FileName;
}
