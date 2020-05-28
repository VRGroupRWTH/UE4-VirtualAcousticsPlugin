// Class to represent a VA Sound Source Signal Buffer for the VA Server

#pragma once

#include <string>								// std::string
#include "Containers/UnrealString.h"			// FString
#include "SharedPointer.h"

class FVASignalBuffer
{
public:

	// Initialization
	FVASignalBuffer(FString FileNameN);


	// Setter
	bool SetLoop(bool bLoopN);
	bool SetSoundTimeOffset(float SoundTimeOffset);
	bool SetSoundBufferAction(int Action);


	// Getter
	std::string GetID() const;
	FString GetFileName() const;
	bool GetLoop() const;
	bool IsValidItem() const;
	int GetSoundBufferAction() const;



protected:

	std::string BufferID;
	FString FileName;
	bool bLoop;
};

typedef TSharedPtr<FVASignalBuffer> FVASignalBufferSharedPtr;
