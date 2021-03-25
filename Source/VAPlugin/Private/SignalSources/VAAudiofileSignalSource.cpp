// Fill out your copyright notice in the Description page of Project Settings.


#include "VAAudiofileSignalSource.h"

#include "VAUtils.h"
#include "VAPlugin.h"

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

void UVAAudiofileSignalSource::Initialize()
{
	if (bInitialized)
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::Initialize()]: Signal source is already initialized, aborting...", true);
		return;
	}

	ID = FVAPlugin::CreateNewBuffer(SoundFile, bLoop, StartingTime);
	if (!IsValidID(ID))
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::Initialize()]: Error creating Audiofile Signal Source", true);
		return;
	}
	bInitialized = true;

	if (!FVAPlugin::SetSoundBufferAction(ID, StartingPlayAction))
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::Initialize()]:  Could not set Play Action during initializing", true);
}

// ****************************************************************** // 
// ******* Bluepring Functions ************************************** //
// ****************************************************************** //

bool UVAAudiofileSignalSource::Play()
{
	return SetPlayAction(EPlayAction::Play);
}

bool UVAAudiofileSignalSource::PlayFromTime(const float fTime)
{
	if (SetPlayBackPosition(fTime))
		return SetPlayAction(EPlayAction::Play);

	return false;
}

bool UVAAudiofileSignalSource::Pause()
{
	return SetPlayAction(EPlayAction::Pause);
}

bool UVAAudiofileSignalSource::Stop()
{
	return SetPlayAction(EPlayAction::Stop);
}

bool UVAAudiofileSignalSource::LoadAudiofile(FString Filename)
{
	return false;
}


// ****************************************************************** // 
// ******* Setter Functions ***************************************** //
// ****************************************************************** //

bool UVAAudiofileSignalSource::SetAudiofile(FString Filename)
{
	std::string NewID = AudiofileManager.GetAudiofileSignalSourceID(Filename);
	if (!IsValidID(NewID))
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::SetAudiofile()]: Audiofile " + Filename + " was loaded incorrectly!", true);
		return false;
	}

	if (ID == NewID)
	{
		//TODO: Should the signal source really be stopped here as in old version?
		//Stop();
		return true;
	}

	if (!CopySignalSourceSettings(NewID))
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::SetAudiofile()]: Could not copy settings to signal source of new audiofile: " + Filename, true);
		return false;
	}
	
	ID = NewID;
	AudiofileChangedEvent.Broadcast(ID);
	return true;
}

bool UVAAudiofileSignalSource::SetLoop(const bool bLoopN)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (bLoop == bLoopN)
	{
		return true;
	}

	bLoop = bLoopN;
	return FVAPlugin::SetSoundBufferLoop(ID, bLoop);
}

bool UVAAudiofileSignalSource::SetPlayBackPosition(const float Time)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}
	return FVAPlugin::SetSoundBufferTime(ID, Time);
}

bool UVAAudiofileSignalSource::SetPlayAction(const int Action)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (int(GetPlayAction()) == Action)
	{
		return true;
	}
	return FVAPlugin::SetSoundBufferAction(ID, EPlayAction::Type(Action));
}



// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

FString UVAAudiofileSignalSource::GetFileName() const
{
	return SoundFile;
}

bool UVAAudiofileSignalSource::GetLoop() const
{
	return bLoop;
}

int UVAAudiofileSignalSource::GetPlayAction() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return -1;
	}
	return FVAPlugin::GetSoundBufferAction(ID);
}

bool UVAAudiofileSignalSource::CopySignalSourceSettings(const std::string& OtherID)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (!FVAPlugin::SetSoundBufferLoop(OtherID, bLoop))
	{
		return false;
	}
	int PlayAction = GetPlayAction();
	if (PlayAction == -1)
	{
		return false;
	}
	return FVAPlugin::SetSoundBufferAction(OtherID, EPlayAction::Type(PlayAction));
}
