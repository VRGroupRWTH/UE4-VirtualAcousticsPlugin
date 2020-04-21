#include "VADirectivity.h"

#include "Containers/Array.h"					// TArray

#include "VAPlugin.h"							// For Server
#include "VADirectivityManager.h"				// For default Directivity


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVADirectivity::FVADirectivity(FString FileNameN) :
	FileName(FileNameN)
{
	CreateNewDirectivity();
}

FVADirectivity::FVADirectivity(FString FileNameN, FString Phoneme) :
	FileName(FileNameN)
{
	TArray<FString> Tmp;
	Tmp.Add(Phoneme);
	Phonemes = Tmp;

	CreateNewDirectivity();
}

FVADirectivity::FVADirectivity(FString FileNameN, TArray<FString> phonemes_) :
	FileName(FileNameN), Phonemes(phonemes_)
{
	CreateNewDirectivity();
}

void FVADirectivity::CreateNewDirectivity()
{
	DirID = FVAPlugin::CreateNewDirectivity(FileName);
	if (DirID == -1)
	{
		FVAUtils::LogStuff("[VADirectivity::createNewDirectivity()] Directivity file " + FileName + " cannot be found!");
		return;
	}

	FVAUtils::LogStuff("created new VADirectivity");
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
	for (const auto Entry : PhonemesN)
	{
		AddPhoneme(Entry);
	}
}


// ****************************************************************** // 
// ******* Check Phonemes ******************************************* //
// ****************************************************************** //

bool FVADirectivity::ContainsPhoneme(const FString Phoneme) const
{
	return Phonemes.Contains(Phoneme);
}

bool FVADirectivity::IsValid() const
{
	return (DirID != -1);
}

void FVADirectivity::LogInfo() const
{
	FString Output = " [Info]  Directivity for: ";

	if (Phonemes.Num() == 0)
	{
		FVAUtils::LogStuff("Directivity has no phonemes");
	}

	for (auto Entry : Phonemes)
	{
		Output.Append(*Entry).Append(",");
	}

	Output.RemoveAt(Output.Len() - 1, 1, true);

	FVAUtils::LogStuff(Output);
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
