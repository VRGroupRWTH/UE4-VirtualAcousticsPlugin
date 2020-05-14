#include "VADirectivityManager.h"

#include "VAUtils.h"

#include "Core.h"
#include "VAHRIR.h"

FVADirectivity* FVADirectivityManager::DefaultDirectivity;


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVADirectivityManager::FVADirectivityManager()
	: ConfigFileName("")
{
}

FVADirectivityManager::~FVADirectivityManager()
{
	Directivities.Reset();
	DefaultDirectivity = nullptr;
}

void FVADirectivityManager::ResetManager()
{
	DefaultDirectivity = nullptr;
	// TODO new
	FVADirectivitySharedPtr NewDirectivity(new FVADirectivity("$(HumanDir)"));
	DefaultDirectivity = NewDirectivity.Get();
	
	Directivities.Empty();
	Directivities.Add(NewDirectivity);
}

bool FVADirectivityManager::ReadConfigFile(const FString ConfigFileNameN)
{
	bool EverythingFine = true;
	ConfigFileName = ConfigFileNameN;

	
	const FString RelativePath = FPaths::ProjectContentDir();
	const FString Dir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

	const FString ConfigFilePath = Dir + ConfigFileName;

	GConfig->UnloadFile(ConfigFilePath);
	GConfig->LoadFile(ConfigFilePath);

	FConfigFile* ConfigFile = GConfig->FindConfigFile(ConfigFilePath);

	if (ConfigFile == nullptr)
	{
		FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Unable to load directivity config file: " +
			ConfigFilePath, true);
		return false;
	}
	FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Config file loaded: " + 
		ConfigFilePath, false);


	FString ListSymbol, MapToSymbol;
	ConfigFile->GetString(TEXT("DirectivityMapping"), TEXT("ListSymbol"), ListSymbol);
	ConfigFile->GetString(TEXT("DirectivityMapping"), TEXT("MapToSymbol"), MapToSymbol);

	if(ListSymbol.Equals(MapToSymbol))
	{
		FVAUtils::OpenMessageBox("[FVADirectivityManager::ReadConfigFile]: ListSymbol and MapToSymbol are equal", true);
	}
	
	TArray<FString> Mapping;
	ConfigFile->GetArray(TEXT("DirectivityMapping"), TEXT("mapping"), Mapping);


	// Format a,b,c:file.data
	TArray<FString> TmpPhonemes;
	FString TmpList,		// for tmp of saving the list of the phonemes "a,b,c"
	        TmpFileName,	// for tmp of saving the file name "file.data"
	        TmpPhoneme;		// for saving each phoneme to bring tmp_list to TArray "a","b","c"


	for (auto EntryMapping : Mapping)
	{
		// Check if mapToSymbol is used
		if (!EntryMapping.Contains(MapToSymbol))
		{
			FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Mapping " + 
				FString(*EntryMapping) + " has a wrong format!(no mapToSymbol)", true);
			EverythingFine = false;
			continue;
		}

		// Check if only one mapToSymbol is used
		if (EntryMapping.Find(MapToSymbol, ESearchCase::CaseSensitive, ESearchDir::FromStart)
			!= EntryMapping.Find(MapToSymbol, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Mapping " + 
				FString(*EntryMapping) + " has a wrong format (multiple mapTopSymbols)", true);
			EverythingFine = false;
			continue;
		}

		FString Tmp = *EntryMapping;

		// Separate for Phoneme List and File Name
		Tmp.Split(MapToSymbol, &TmpList, &TmpFileName);

		if (TmpList.IsEmpty() || TmpFileName.IsEmpty())
		{
			FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Mapping " + 
				FString(*EntryMapping) + " has a wrong format! (no list of phonemes or file name)", true);
			EverythingFine = false;
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
		FString Output = "[FVADirectivityManager::ReadConfigFile()]: Mapping created: ";
		for (auto EntryPhoneme : TmpPhonemes)
		{
			Output.Append(EntryPhoneme + " | ");
		}
		Output.Append(" --> " + TmpFileName);
		FVAUtils::LogStuff(Output, false);


		// Create Directivities
		FVADirectivity* TmpDir = GetDirectivityByFileName(TmpFileName);

		// If there is no Dir from that file
		if (TmpDir == nullptr)
		{
			FVAUtils::LogStuff("[FVADirectivityManager::ReadConfigFile()]: Could not create Directivity from file " + 
				TmpFileName, true);
			EverythingFine = false;
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

	if (!EverythingFine)
	{
		FVAUtils::OpenMessageBox("[FVADirectivityManager::ReadConfigFile()]: There is an error in the mapping file, see logs for more information", true);
	}
	
	return EverythingFine;
}


// ****************************************************************** // 
// ******* Get Directivities **************************************** //
// ****************************************************************** //

FVADirectivity* FVADirectivityManager::GetDirectivityByPhoneme(const FString Phoneme) const
{
	// Search for Directivity
	for (auto Entry : Directivities)
	{
		if (Entry.Get()->ContainsPhoneme(Phoneme))
		{
			return Entry.Get();
		}
	}
	FVAUtils::LogStuff("[FVADirectivityManager::GetDirectivityByPhoneme()]: Directivity for phoneme " + 
		Phoneme + " cannot be found! Using defaultDir", true);

	// If there is a default Directivity from ini file
	return GetDefaultDirectivity();
}

FVADirectivity* FVADirectivityManager::GetDirectivityByFileName(const FString FileName)
{
	// Search for already existing Directivity
	if (Directivities.Num() > 0)
	{
		for (auto Entry : Directivities)
		{
			if (Entry.Get()->GetFileName() == FileName)
			{
				FVAUtils::LogStuff("[FVADirectivityManager::getDirectivityByFileName()]: Directivity from file " + 
					FileName + " was found!", false);

				return Entry.Get();
			}
		}
	}

	FVAUtils::LogStuff("[FVADirectivityManager::GetDirectivityByFileName()]: Directivity from file " + 
		FileName + " cannot be found! Creating one now.", false);

	// TODO new 
	FVADirectivitySharedPtr NewDirectivity(new FVADirectivity(FileName));
	
	if (NewDirectivity.IsValid() && NewDirectivity->IsValidItem())
	{
		FVAUtils::LogStuff("[FVAHeadRelatedIRManager::GetDirectivityByFileName()]: Directivity from file "+ 
			FileName + " is created!", false);
		Directivities.Add(NewDirectivity);
		return NewDirectivity.Get();
	}
	FVAUtils::LogStuff("[FVADirectivityManager::GetDirectivityByFileName()]: Directivity from file " + 
		FileName + " cannot be created!", true);

	// Return default one
	return GetDefaultDirectivity();
}



FVADirectivity* FVADirectivityManager::GetDefaultDirectivity()
{
	if (DefaultDirectivity != nullptr)
	{
		return DefaultDirectivity;
	}
	FVAUtils::LogStuff("[FVADirectivityManager::GetDefaultDirectivity()]: No default directivity found. " + 
		FString("Please have a scene loaded so a ReceiverActor exists handling a DirManager containing the defaultDir."), true);
	return nullptr;
}


// ****************************************************************** // 
// ******* Getter Function ****************************************** //
// ****************************************************************** //


FString FVADirectivityManager::GetFileName() const
{
	return ConfigFileName;
}

bool FVADirectivityManager::IsValid() const
{
	return bool(ConfigFileName != "");
}

void FVADirectivityManager::PrintDirMapping() const
{
	for (auto Entry : Directivities)
	{
		Entry.Get()->LogInfo();
	}
}
