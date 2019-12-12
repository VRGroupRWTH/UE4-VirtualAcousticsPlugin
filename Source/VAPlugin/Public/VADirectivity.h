// Class to represent a Directivity for the VA Server

#pragma once



class VADirectivity
{


public:
	VADirectivity(FString fileName_);
	VADirectivity(FString fileName_, FString phoneme);
	VADirectivity(FString fileName_, TArray<FString> phonemes);

	void addPhoneme(FString phoneme);
	void addPhoneme(TArray<FString> phoneme);
	
	int getID();
	
	void logInfo();
	

	bool containsPhoneme(FString phoneme);
	
	bool isValid();

	FString getFileName();

private:
	int dirID;
	FString fileName;
	TArray<FString> phonemes;

};