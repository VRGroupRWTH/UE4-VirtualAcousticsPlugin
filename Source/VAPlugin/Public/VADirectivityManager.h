// Class to manage all the Directivities

#pragma once

#include "VADirectivity.h"

class VADirectivity;

class VADirectivityManager
{
public:
	VADirectivityManager();
	VADirectivityManager(FString configFileName_);

	void readConfigFile(FString configFileName_);

	VADirectivity* getDirectivityByPhoneme(FString phoneme);

	bool isValid();

	void printDirMapping();
	
private:
	TArray<VADirectivity*> directivities;

	VADirectivity* defaultDir;

	FString configFileName;

};