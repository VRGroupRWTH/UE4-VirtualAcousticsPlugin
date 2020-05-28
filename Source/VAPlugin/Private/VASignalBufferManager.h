// Class to manage all the Signal Buffer

#pragma once

#include "VASignalBuffer.h"

class FVASignalBufferManager
{
public:

	// Initialization
	FVASignalBufferManager();
	~FVASignalBufferManager();

	
	// Handle Buffer
	FVASignalBuffer* GetBufferByFileName(FString FileName);

private:
	TArray<FVASignalBufferSharedPtr> Buffers;
};
