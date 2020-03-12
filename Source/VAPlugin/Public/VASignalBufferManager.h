// Class to manage all the Signal Buffer

#pragma once

#include "VASignalBuffer.h"

class VASignalBufferManager
{
public:
	VASignalBufferManager();
	~VASignalBufferManager();

	VASignalBuffer* getBufferByFileName(FString fileName_);

private:
	TArray<VASignalBuffer*> buffers;
};
