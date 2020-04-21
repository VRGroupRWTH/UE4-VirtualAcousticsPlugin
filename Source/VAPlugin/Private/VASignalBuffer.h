// Class to represent a VA Sound Source Signal Buffer for the VA Server

#pragma once

#include <xstring>

#include "Containers/UnrealString.h"			// FString

class FVASignalBuffer
{
public:

	// Initialization
	FVASignalBuffer(FString FileNameN);


	// Setter
	void SetLoop(bool bLoopN);
	void SetSoundTimeOffset(float SoundTimeOffset);
	void SetSoundBufferAction(int Action);


	// Getter
	std::string GetID() const;
	FString GetFileName() const;
	bool GetLoop() const;
	bool IsValid() const;
	int GetSoundBufferAction() const;



protected:

	std::string BufferID;
	FString FileName;
	bool bLoop;
};
