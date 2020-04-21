#include "FVASignalBuffer.h"

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

void FVASignalBuffer::SetLoop(const bool bLoopN)
{
	if (bLoop == bLoopN)
	{
		return;
	}

	bLoop = bLoopN;
	FVAPlugin::SetSoundBufferLoop(BufferID, bLoop);
}

void FVASignalBuffer::SetSoundTimeOffset(const float SoundTimeOffset)
{
	FVAPlugin::SetSoundBufferTime(BufferID, SoundTimeOffset);
}

void FVASignalBuffer::SetSoundBufferAction(const int Action)
{
	FVAPlugin::SetSoundBufferAction(BufferID, static_cast<EPlayAction>(Action));
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

bool FVASignalBuffer::IsValid() const
{
	return bool(FString(BufferID.c_str()).Equals(FString("-1")));
}

int FVASignalBuffer::GetSoundBufferAction() const
{
	return FVAPlugin::GetSoundBufferAction(BufferID);
}
