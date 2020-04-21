#include "FVASignalBufferManager.h"

#include "FVAUtils.h"


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
		for (auto Entry : Buffers)
		{
			if (Entry->GetFileName() == FileName)
			{
				FVAUtils::LogStuff("[VASignalBufferManager::getBufferByFileName()] Buffer from file " + FileName +
					" was found!");

				return Entry;
			}
		}
	}


	FVAUtils::LogStuff(
		"[VASignalBufferManager::getBufferByFileName()] Buffer from file " + FileName +
		" cannot be found! Creating one now...");

	// Create a new Buffer
	FVASignalBuffer* NewBuffer = new FVASignalBuffer(FileName);
	if (NewBuffer != nullptr && NewBuffer->IsValid())
	{
		FVAUtils::LogStuff("[VASignalBufferManager::getBufferByFileName()] Buffer from file " + FileName +
			" is created!");
		Buffers.Add(NewBuffer);
	}
	FVAUtils::LogStuff(
		"[VASignalBufferManager::getBufferByFileName()] Buffer from file " + FileName +
		" cannot be created properly!");

	return NewBuffer;
}
