// Fill out your copyright notice in the Description page of Project Settings.


#include "SignalSources/VAAudiofileSignalSource.h"

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

	if (!SetAudiofile(Filename))
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::Initialize()]: Error creating Audiofile Signal Source", true);
		return;
	}
	bInitialized = true;
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

bool UVAAudiofileSignalSource::PreLoadAudiofile(FString AudioFilename)
{
	return AudiofileManager.PreLoadAudiofile(AudioFilename);
}


// ****************************************************************** // 
// ******* Setter Functions ***************************************** //
// ****************************************************************** //

bool UVAAudiofileSignalSource::SetAudiofile(FString AudioFilename)
{
	std::string NewID = AudiofileManager.GetAudiofileSignalSourceID(AudioFilename);
	if (!IsValidID(NewID))
	{
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::SetAudiofile()]: Audiofile " + AudioFilename + " was loaded incorrectly!", true);
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
		FVAUtils::LogStuff("[UVAAudiofileSignalSource::SetAudiofile()]: Could not copy settings to signal source of new audiofile: " + AudioFilename, true);
		return false;
	}
	
	Filename = AudioFilename;
	ID = NewID;

	if (bInitialized) //Broadcast is only required after initialization (after BeginPlay())
	{
		AudiofileChangedEvent.Broadcast(ID);
	}
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
	return FVAPlugin::SetSignalSourceBufferLooping(ID, bLoop);
}

bool UVAAudiofileSignalSource::SetPlayBackPosition(const float Time)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}
	return FVAPlugin::SetSignalSourceBufferPlaybackPosition(ID, Time);
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
	return FVAPlugin::SetSignalSourceBufferPlayAction(ID, EPlayAction::Type(Action));
}



// ****************************************************************** // 
// ******* Getter Functions ***************************************** //
// ****************************************************************** //

FString UVAAudiofileSignalSource::GetFilename() const
{
	return Filename;
}

bool UVAAudiofileSignalSource::GetLoop() const
{
	return bLoop;
}

EPlayAction::Type UVAAudiofileSignalSource::GetPlayActionEnum() const
{
	return EPlayAction::Type(GetPlayAction());
}

int UVAAudiofileSignalSource::GetPlayAction() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return -1;
	}
	return FVAPlugin::GetSignalSourceBufferPlayAction(ID);
}

bool UVAAudiofileSignalSource::CopySignalSourceSettings(const std::string& OtherID)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (!FVAPlugin::SetSignalSourceBufferLooping(OtherID, bLoop))
	{
		return false;
	}

	int PlayAction = StartingPlayAction;
	if (bInitialized)
	{
		PlayAction = GetPlayAction();
		if (PlayAction == -1)
		{
			return false;
		}
	}
	return FVAPlugin::SetSignalSourceBufferPlayAction(OtherID, EPlayAction::Type(PlayAction));
}
