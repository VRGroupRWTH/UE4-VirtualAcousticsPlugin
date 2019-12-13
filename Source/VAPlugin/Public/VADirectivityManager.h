// Class to manage all the Directivities

#pragma once

#include "VADirectivity.h"

class VADirectivityManager
{
public:
	VADirectivityManager();
	VADirectivityManager(FString configFileName_);

	void readConfigFile(FString configFileName_);


	void reset();

	VADirectivity* getDirectivityByPhoneme(FString phoneme);
	VADirectivity* getDirectivityByFileName(FString fileName_);

	bool isValid();

	void printDirMapping();

	static VADirectivity* getDefaultReceiverDirectivity();
	static VADirectivity* getDefaultSourceDirectivity();
	
private:
	TArray<VADirectivity*> directivities;

	VADirectivity* defaultDir;

	FString configFileName;

	static VADirectivity* defaultReceiverDirectivity;
	static VADirectivity* defaultSourceDirectivity;

};