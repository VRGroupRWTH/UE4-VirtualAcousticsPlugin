#include "VASignalBuffer.h"

#include "VAPlugin.h"							// For Server


VASignalBuffer::VASignalBuffer(FString fileName_) :
	fileName(fileName_)
{
	bufferID = FVAPluginModule::createNewBuffer(fileName);
}

void VASignalBuffer::setLoop(bool loop_)
{
	if (loop == loop_) {
		return;
	}

	loop = loop_;
	FVAPluginModule::setSoundBufferLoop(bufferID, loop);
}

bool VASignalBuffer::getLoop()
{
	return loop;
}

void VASignalBuffer::setSoundTimeOffset(float soundTimeOffset)
{
	FVAPluginModule::setSoundBufferTime(bufferID, soundTimeOffset);
}

void VASignalBuffer::setSoundBufferAction(int action)
{
	FVAPluginModule::setSoundBufferAction(bufferID, static_cast<EPlayAction>(action));
}

int VASignalBuffer::getSoundBufferAction()
{
	return FVAPluginModule::getSoundBufferAction(bufferID);
}

std::string VASignalBuffer::getID()
{
	return bufferID;
}


bool VASignalBuffer::isValid()
{
	return valid;
}

FString VASignalBuffer::getFileName()
{
	return fileName;
}
