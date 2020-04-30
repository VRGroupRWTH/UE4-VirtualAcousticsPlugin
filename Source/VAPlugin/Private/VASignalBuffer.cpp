#include "VASignalBuffer.h"

#include "VAPlugin.h"							// For Server


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASignalBuffer::FVASignalBuffer(const FString FileNameN) :
	FileName(FileNameN), bLoop(false)
{
	BufferID = FVAPlugin::CreateNewBuffer(FileName);
}


// ****************************************************************** // 
// ******* Setter Functions ***************************************** //
// ****************************************************************** //

bool FVASignalBuffer::SetLoop(const bool bLoopN)
{
	if (bLoop == bLoopN)
	{
		return true;
	}

	bLoop = bLoopN;
	return FVAPlugin::SetSoundBufferLoop(BufferID, bLoop);
}

bool FVASignalBuffer::SetSoundTimeOffset(const float SoundTimeOffset)
{
	return FVAPlugin::SetSoundBufferTime(BufferID, SoundTimeOffset);
}

bool FVASignalBuffer::SetSoundBufferAction(const int Action)
{
	return FVAPlugin::SetSoundBufferAction(BufferID, EPlayAction(Action));
}



// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

std::string FVASignalBuffer::GetID() const
{
	return BufferID;
}

FString FVASignalBuffer::GetFileName() const
{
	return FileName;
}

bool FVASignalBuffer::GetLoop() const
{
	return bLoop;
}

bool FVASignalBuffer::IsValidItem() const
{
	return !bool(FString(BufferID.c_str()).Equals(FString("-1")));
}

int FVASignalBuffer::GetSoundBufferAction() const
{
	return FVAPlugin::GetSoundBufferAction(BufferID);
}
