// Class to manage all the Directivities

#pragma once

#include "Directivity/VADirectivity.h"

class FVADirectivityManager
{
public:
	
	// Initialization
	FVADirectivityManager();
	~FVADirectivityManager();
	
	void ResetManager();
	bool ReadConfigFile(FString ConfigFileNameN);

	
	// Get Directivities
	FVADirectivity* GetDirectivityByPhoneme(FString Phoneme) const;
	FVADirectivity* GetDirectivityByFileName(FString FileName);
	
	static FVADirectivity* GetDefaultDirectivity();



	
	// Getter Function
	FString GetFileName() const;
	void PrintDirMapping() const;
	bool IsValid() const;

	


private:
	TArray<FVADirectivitySharedPtr> Directivities;

	FString ConfigFileName;

	// Default Directivity, can be overloaded by config file
	static FVADirectivity* DefaultDirectivity;
};
