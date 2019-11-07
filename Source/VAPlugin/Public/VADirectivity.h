// Class to represent a Directivity for the VA Server

#pragma once

class VADirectivity
{
public:
	VADirectivity();
	VADirectivity(int dirID_, TArray<FString> phonemes);
	VADirectivity(FString fileName, FString phoneme);
	VADirectivity(FString fileName, TArray<FString> phonemes);
	
	void addPhoneme(FString phoneme);
	
	int getID();
	
	void logInfo();
	
	bool containsPhoneme(FString phoneme);
	
	bool isValid();

private:
	int dirID;
	TArray<FString> phonemes;

};