#include "VASignalBufferManager.h"

#include "VAUtils.h"



VASignalBufferManager::VASignalBufferManager()
{
}

VASignalBufferManager::~VASignalBufferManager()
{
}




VASignalBuffer* VASignalBufferManager::getBufferByFileName(FString fileName_)
{
	// Find already existing Buffer
	if (!buffers.Num()) {
		for (auto entry : buffers) {
			if (entry->getFileName() == fileName_) {
				FString output = "[VASignalBufferManager::getBufferByFileName()] Buffer from file " + fileName_ + " was found!";
				VAUtils::logStuff(output);

				return entry;
			}
		}
	}


	VAUtils::logStuff("[VASignalBufferManager::getBufferByFileName()] Buffer from file " + fileName_ + " cannot be found! Creating one now...");

	// Create a new Buffer
	VASignalBuffer* new_buffer = new VASignalBuffer(fileName_);
	if (new_buffer != nullptr) {
		FString output = "[VASignalBufferManager::getBufferByFileName()] Buffer from file " + fileName_ + " is created!";
		VAUtils::logStuff(output);
		buffers.Add(new_buffer);
		return new_buffer;
	}
	else {
		VAUtils::logStuff("[VASignalBufferManager::getBufferByFileName()] Buffer from file " + fileName_ + " cannot be created properly!");
	}

	// Return Default Buffer
	return nullptr;
}




