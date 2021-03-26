// Class to represent a Directivity for the VA Server

#pragma once

#include "Containers/UnrealString.h"			// FString
#include "Templates/SharedPointer.h"

class FVADirectivity
{
public:
	
	// Constructor
	FVADirectivity(FString FileNameN);
	FVADirectivity(FString FileNameN, FString Phoneme);
	FVADirectivity(FString FileNameN, TArray<FString> Phonemes);

	
	// Phoneme handling
	void AddPhoneme(FString Phoneme);
	void AddPhoneme(TArray<FString> PhonemesN);

	
	// Check Phonemes
	bool ContainsPhoneme(FString Phoneme) const;
	bool IsValidItem() const;
	void LogInfo() const;

	
	// Getter Function
	int GetID() const;
	FString GetFileName() const;

protected:
	void CreateNewDirectivity();

	int DirID;
	FString FileName;
	TArray<FString> Phonemes;
};

typedef TSharedPtr<FVADirectivity> FVADirectivitySharedPtr;