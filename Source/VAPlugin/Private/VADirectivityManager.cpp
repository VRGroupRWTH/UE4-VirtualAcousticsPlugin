#include "VADirectivityManager.h"

#include "FVAUtils.h"

#include "Core.h"
#include "Interfaces/IPluginManager.h"

FVADirectivity* FVADirectivityManager::DefaultDirectivity = nullptr;


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVADirectivityManager::FVADirectivityManager()
	: ConfigFileName("")
{
}

FVADirectivityManager::~FVADirectivityManager()
{
	DefaultDirectivity = nullptr;
}

void FVADirectivityManager::ResetManager()
{
	if (DefaultDirectivity != nullptr)
	{
		delete DefaultDirectivity;
	}
	DefaultDirectivity = new FVADirectivity(FString("$(HumanDir)"));

	Directivities.Empty();
	Directivities.Add(DefaultDirectivity);
}

bool FVADirectivityManager::ReadConfigFile(const FString ConfigFileNameN)
{
	ConfigFileName = ConfigFileNameN;

	Directivities.Add(DefaultDirectivity);
	
	const FString RelativePath = FPaths::ProjectContentDir();
	const FString Dir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

	const FString ConfigFilePath = Dir + ConfigFileName;

	GConfig->UnloadFile(ConfigFilePath);
	GConfig->LoadFile(ConfigFilePath);

	FConfigFile* ConfigFile = GConfig->FindConfigFile(ConfigFilePath);

	if (ConfigFile == nullptr)
	{
		FVAUtils::LogStuff(
			FString("[VADirectivityManager::ReadConfigFile()] - Unable to load directivity config file: ") +
			ConfigFilePath);
		return false;
	}
	FVAUtils::LogStuff(FString("[VADirectivityManager::ReadConfigFile()] - Config file loaded: ") + ConfigFilePath);


	FString ListSymbol, MapToSymbol;
	ConfigFile->GetString(TEXT("DirectivityMapping"), TEXT("listSymbol"), ListSymbol);
	ConfigFile->GetString(TEXT("DirectivityMapping"), TEXT("mapToSymbol"), MapToSymbol);

	TArray<FString> Mapping;
	ConfigFile->GetArray(TEXT("DirectivityMapping"), TEXT("mapping"), Mapping);


	// Format a,b,c:file.data
	TArray<FString> TmpPhonemes;
	FString TmpList,		// for tmp of saving the list of the phonemes "a,b,c"
	        TmpFileName,	// for tmp of saving the file name "file.data"
	        TmpPhoneme;		// for saving each phoneme to bring tmp_list to TArray "a","b","c"


	for (auto Entry : Mapping)
	{
		if (!Entry.Contains(MapToSymbol))
		{
			FVAUtils::LogStuff("Directivity" + FString(*Entry) + " has a wrong format!(no mapToSymbol)");
			continue;
		}

		FString Tmp = *Entry;

		// Separate for Phoneme List and File Name
		Tmp.Split(MapToSymbol, &TmpList, &TmpFileName);

		if (TmpList.IsEmpty() || TmpFileName.IsEmpty())
		{
			FVAUtils::LogStuff(
				"Directivity" + FString(*Entry) + " has a wrong format! (no list of phonemes or file name)");
			continue;
		}

		// Convert tmp_list into TArray of all phonemes
		TmpPhonemes.Empty();
		while (TmpList.Contains(ListSymbol))
		{
			TmpList.Split(ListSymbol, &TmpPhoneme, &TmpList);
			TmpPhonemes.Add(TmpPhoneme);
		}
		TmpPhonemes.Add(TmpList);


		// Log output
		FString Output = "Mapping from: ";
		for (auto Entry2 : TmpPhonemes)
		{
			Output.Append(Entry2 + ",");
		}
		Output.Append(" --> " + TmpFileName);
		FVAUtils::LogStuff(Output);


		// Create Directivities
		FVADirectivity* TmpDir = GetDirectivityByFileName(TmpFileName);

		// If there is no Dir from that file
		if (TmpDir == nullptr)
		{
			FVAUtils::LogStuff("FVADirectivityManager::ReadConfigFile() - Could not create Directivity from file " + TmpFileName);
		}
		else
		{
			TmpDir->AddPhoneme(TmpPhonemes);
		}

		// If it is the default directivity
		if (TmpPhonemes.Contains("default"))
		{
			DefaultDirectivity = TmpDir;
		}
	}
	return true;
}


// ****************************************************************** // 
// ******* Get Directivities **************************************** //
// ****************************************************************** //

FVADirectivity* FVADirectivityManager::GetDirectivityByPhoneme(const FString Phoneme) const
{
	// Search for Directivity
	for (auto Entry : Directivities)
	{
		if (Entry->ContainsPhoneme(Phoneme))
		{
			return Entry;
		}
	}
	FVAUtils::LogStuff(
		"[VADirectivityManager::GetDirectivityByPhoneme()] Directivity for phoneme " + Phoneme +
		" cannot be found! Using defaultDir");

	// If there is a default Directivity from ini file
	return GetDefaultDirectivity();
}

FVADirectivity* FVADirectivityManager::GetDirectivityByFileName(const FString FileName)
{
	// Search for already existing Directivity
	if (!Directivities.Num())
	{
		for (auto Entry : Directivities)
		{
			if (Entry->GetFileName() == FileName)
			{
				FVAUtils::LogStuff(
					"[VADirectivityManager::getDirectivityByFileName()] Directivity from file " + FileName +
					" was found!");

				return Entry;
			}
		}
	}

	FVAUtils::LogStuff(
		"[VADirectivityManager::GetDirectivityByFileName()] Directivity from file " + FileName +
		" cannot be found! Creating one now...");

	// Create a new one TODO new 
	FVADirectivity* NewDirectivity = new FVADirectivity(FileName);
	if (NewDirectivity != nullptr)
	{
		Directivities.Add(NewDirectivity);
		return NewDirectivity;
	}
	FVAUtils::LogStuff(
		"[VADirectivityManager::GetDirectivityByFileName()] Directivity from file " + FileName +
		" cannot be created!");

	// Return default one
	return GetDefaultDirectivity();
}

FVADirectivity* FVADirectivityManager::GetDefaultDirectivity()
{
	if (DefaultDirectivity == nullptr)
	{
		DefaultDirectivity = new FVADirectivity(FString("$(HumanDir)"));
	}

	return DefaultDirectivity;
}


// ****************************************************************** // 
// ******* Getter Function ****************************************** //
// ****************************************************************** //

FString FVADirectivityManager::GetFileName() const
{
	return ConfigFileName;
}


// ****************************************************************** // 
// ******* Get Information ****************************************** //
// ****************************************************************** //

bool FVADirectivityManager::IsValid() const
{
	return bool(ConfigFileName != "");
}

void FVADirectivityManager::PrintDirMapping() const
{
	for (auto Entry : Directivities)
	{
		Entry->LogInfo();
	}
}
