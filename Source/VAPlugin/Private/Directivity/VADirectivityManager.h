// Class to manage all the Directivities

#pragma once

#include "Directivity/VADirectivity.h"

#include "Templates/SharedPointer.h"

class FVADirectivityManager
{
public:
	
	// Initialization
	FVADirectivityManager();
	~FVADirectivityManager();
	
	void ResetManager();
	bool ReadConfigFile(FString ConfigFileNameN);

	
	// Get Directivities
	FVADirectivitySharedPtr GetDirectivityByPhoneme(FString Phoneme) const;
	FVADirectivitySharedPtr GetDirectivityByFileName(FString FileName);
	
	static FVADirectivitySharedPtr GetDefaultDirectivity();



	
	// Getter Function
	FString GetFileName() const;
	void PrintDirMapping() const;
	bool IsValid() const;

	


private:
	TArray<FVADirectivitySharedPtr> Directivities;

	FString ConfigFileName;

	// Default Directivity, can be overloaded by config file
	static FVADirectivitySharedPtr DefaultDirectivity;
};
