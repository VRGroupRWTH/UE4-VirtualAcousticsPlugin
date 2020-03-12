// Class to represent a Directivity for the VA Server

#pragma once


class VAHRIR
{
public:
	VAHRIR(FString fileName_);

	int getID();

	bool isValid();

	FString getFileName();

protected:

	void createNewHRIR();

	int hrirID;
	FString fileName;
};
