// Class to manage all the Directivities

#pragma once

#include "VADirectivity.h"

class VADirectivityManager
{
public:
	VADirectivityManager();
	~VADirectivityManager();

	void reset();

	void readConfigFile(FString configFileName_);


	VADirectivity* getDirectivityByPhoneme(FString phoneme);
	VADirectivity* getDirectivityByFileName(FString fileName_);

	bool isValid();

	void printDirMapping();

	static VADirectivity* getDefaultDirectivity();

	FString getFileName();

private:
	TArray<VADirectivity*> directivities;


	FString configFileName;

	// Default Directivity, can be overloaded by config file
	static VADirectivity* defaultDirectivity;
};
