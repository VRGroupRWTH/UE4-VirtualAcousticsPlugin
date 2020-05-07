#include "VASignalBufferManager.h"

#include "VAUtils.h"


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASignalBufferManager::FVASignalBufferManager()
{
}

FVASignalBufferManager::~FVASignalBufferManager()
{
}

// ****************************************************************** // 
// ******* Handle Buffer ******************************************** //
// ****************************************************************** //

FVASignalBuffer* FVASignalBufferManager::GetBufferByFileName(const FString FileName)
{
	// Find already existing Buffer
	if (!Buffers.Num())
	{
		for (auto EntryBuffer : Buffers)
		{
			if (EntryBuffer.Get()->GetFileName() == FileName)
			{
				FVAUtils::LogStuff("[FVASignalBufferManager::getBufferByFileName()]: Buffer from file " + 
					FileName + " was found!", false);

				return EntryBuffer.Get();
			}
		}
	}


	FVAUtils::LogStuff("[FVASignalBufferManager::getBufferByFileName()]: Buffer from file " + 
		FileName + " cannot be found! Creating one now...", false);

	// TODO new
	FVASignalBufferSharedPtr NewBuffer = MakeShared<FVASignalBuffer>(FileName);

	if (NewBuffer.IsValid() && NewBuffer->IsValidItem())
	{
		FVAUtils::LogStuff("[FVASignalBufferManager::getBufferByFileName()]: Buffer from file " + 
			FileName + " is created!", false);
		Buffers.Add(NewBuffer);
		return NewBuffer.Get();
	}
	
	FVAUtils::LogStuff("[FVASignalBufferManager::getBufferByFileName()]: Buffer from file " + 
		FileName + " cannot be created properly!", true);

	return nullptr;
}
