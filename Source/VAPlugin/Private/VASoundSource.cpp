#include "VASoundSource.h"

#include "VASourceComponent.h"
#include "VAPlugin.h"

int FVASoundSource::Counter = 0;

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSource::FVASoundSource(UVASourceComponent* ParentComponent) :
	ParentComponent(ParentComponent)
{
	bPlaying = false;

	bShowCones = FVAPlugin::GetDebugMode();
	bHandleReflections = ParentComponent->GetHandleReflections();
	bLoop = ParentComponent->GetLoop();
	Power = ParentComponent->GetSoundPower();

	const float SoundTimeOffset = ParentComponent->GetSoundTimeOffset();

	Position = ParentComponent->GetPosition();
	Rotation = ParentComponent->GetRotation();

	std::string FileName(TCHAR_TO_UTF8(*ParentComponent->GetSoundFile()));
	const std::string Name = "SoundSource";

	if (FVAPlugin::GetIsMaster())
	{
		ActiveBuffer = BufferManager.GetBufferByFileName(ParentComponent->GetSoundFile());

		if (ActiveBuffer == nullptr)
		{
			FVAUtils::LogStuff("[VASoundSource::VASoundSource(UVASourceComponent*)] Error initializing Buffer", true);
			return;
		}

		ActiveBuffer->SetLoop(bLoop);
		
		// Only Change if necessary
		if (SoundTimeOffset > 0.0f)
		{
			ActiveBuffer->SetSoundTimeOffset(SoundTimeOffset);
		}

		SoundSourceID = FVAPlugin::CreateNewSoundSource(ActiveBuffer->GetID(), Name, Position, Rotation, Power);
		if (SoundSourceID == -1)
		{
			FVAUtils::LogStuff("[VASoundSource::VASoundSource(UVASourceComponent*)] Error initializing soundSource",
			                  true);
			return;
		}

		FVAPlugin::SetSoundSourcePower(SoundSourceID, ParentComponent->GetSoundPower());
	}

	// Show graphical representation
	SoundSourceRepresentation = ParentComponent->GetWorld()->SpawnActor<AVASoundSourceRepresentation>(
		AVASoundSourceRepresentation::StaticClass());


	SoundSourceRepresentation->SetPosition(ParentComponent->GetPosition());
	SoundSourceRepresentation->SetRotation(ParentComponent->GetRotation());
	SoundSourceRepresentation->SetVisibility(bShowCones);

	if (bHandleReflections)
	{
		TArray<AVAReflectionWall*> WallArray = FVAPlugin::GetReflectionWalls();
		for (auto EntryWall : WallArray)
		{
			std::string WallName(TCHAR_TO_UTF8(*EntryWall->GetName()));
			const std::string NameTmp = Name + "_R_" + WallName;

			std::string ActiveBufferName;
			
			if (FVAPlugin::GetIsMaster())
			{
				ActiveBufferName = ActiveBuffer->GetID();
			}
			else
			{
				ActiveBufferName = "BufferNameNotNeededOnSlave";
			}
			
			const float R = EntryWall->GetReflectionValueR();
			const float PowerR = Power * R * R;
			Reflections.Add(new FVASoundSourceReflection(this, EntryWall, ActiveBufferName, NameTmp, Position, Rotation, PowerR));
		}
	}
}



// ****************************************************************** // 
// ******* Setter *************************************************** //
// ****************************************************************** //


void FVASoundSource::SetPosition(const FVector PosN)
{
	Position = PosN;

	if (FVAPlugin::GetIsMaster())
	{
		FVAPlugin::SetSoundSourcePosition(SoundSourceID, Position);
	}

	SoundSourceRepresentation->SetPosition(Position);

	
	if (bHandleReflections)
	{
		for (auto EntryReflections : Reflections)
		{
			EntryReflections->SetPosition(Position);
		}
	}
}

void FVASoundSource::SetRotation(const FRotator RotN)
{
	Rotation = RotN;

	if (FVAPlugin::GetIsMaster())
	{
		FVAPlugin::SetSoundSourceRotation(SoundSourceID, Rotation);
	}

	SoundSourceRepresentation->SetRotation(Rotation);

	if (bHandleReflections)
	{
		for (auto EntryReflections : Reflections)
		{
			EntryReflections->SetRotation(Rotation);
		}
	}
}

void FVASoundSource::SetVisibility(const bool VisN)
{
	if (bShowCones == VisN)
	{
		return;
	}
	
	bShowCones = VisN;

	SoundSourceRepresentation->SetVisibility(bShowCones);

	if (bHandleReflections && Reflections.Num() != 0)
	{
		for (auto EntryReflections : Reflections)
		{
			EntryReflections->SetVisibility(bShowCones);
		}
	}
}

void FVASoundSource::SetDirectivity(FVADirectivity* DirN)
{
	Directivity = DirN;

	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, DirN->GetID());

	if (bHandleReflections)
	{
		for (auto EntryReflections : Reflections)
		{
			EntryReflections->SetDirectivity(DirN);
		}
	}
}

void FVASoundSource::SetPlayAction(const int ActionN) const
{
	if (!FVAPlugin::GetIsMaster() || GetPlayState() == ActionN)
	{
		return;
	}

	ActiveBuffer->SetSoundBufferAction(ActionN);
}

void FVASoundSource::SetSoundTime(const float TimeN) const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	ActiveBuffer->SetSoundTimeOffset(TimeN);
}

void FVASoundSource::SetLoop(const bool bLoopN)
{
	if (!FVAPlugin::GetIsMaster() || bLoop == bLoopN)
	{
		return;
	}
	
	bLoop = bLoopN;

	ActiveBuffer->SetLoop(bLoopN);
}

void FVASoundSource::SetPower(const float PowerN)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	if (Power == PowerN)
	{
		return;
	}
	
	Power = PowerN;

	FVAPlugin::SetSoundSourcePower(SoundSourceID, PowerN);
	for (auto EntryReflections : Reflections)
	{
		EntryReflections->SetPowerAccordingOrigSource();
	}
}

void FVASoundSource::PlaySound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	SetPlayAction(Play);
}

void FVASoundSource::StopSound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	SetPlayAction(Stop);
}

void FVASoundSource::PauseSound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	SetPlayAction(Pause);
}

void FVASoundSource::PlaySoundFromSecond(const float TimeN) const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	SetSoundTime(TimeN);

	PlaySound();
}

void FVASoundSource::MuteSound(const bool MutedN)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return;
	}

	FVAPlugin::SetSoundSourceMuted(SoundSourceID, MutedN);

	for (auto EntryReflections : Reflections)
	{
		FVAPlugin::SetSoundSourceMuted(EntryReflections->GetSoundSourceID(), MutedN);
	}
}

bool FVASoundSource::SetNewSound(FString SoundFileN)
{
	// create new buffer
	FVASignalBuffer* TmpBuffer = BufferManager.GetBufferByFileName(SoundFileN);

	// Check if is valid
	if (TmpBuffer == nullptr)
	{
		FVAUtils::LogStuff("[VASoundSource::SetNewSound()] Buffer from file " + SoundFileN + " was loaded incorrectly!");
		return false;
	}

	// stop 
	StopSound();

	// If the same Buffer is already loaded only stop sound (prev)
	if (TmpBuffer == ActiveBuffer)
	{
		return true;
	}

	const std::string NewBufferID = TmpBuffer->GetID();

	// Link to source buffer
	FVAPlugin::SetNewBufferForSoundSource(SoundSourceID, NewBufferID);
	for (auto EntryReflections : Reflections)
	{
		FVAPlugin::SetNewBufferForSoundSource(EntryReflections->GetSoundSourceID(), NewBufferID);
	}

	ActiveBuffer = TmpBuffer;

	return true;
}

bool FVASoundSource::LoadNewSound(FString SoundFileN)
{
	FVASignalBuffer* TmpBuffer = BufferManager.GetBufferByFileName(SoundFileN);

	if (TmpBuffer != nullptr)
	{
		return true;
	}

	return false;
}

// ****************************************************************** // 
// ******* Getter *************************************************** //
// ****************************************************************** //

FVector FVASoundSource::GetPosition() const
{
	return Position;
}

FRotator FVASoundSource::GetRotation() const
{
	return Rotation;
}

bool FVASoundSource::GetVisibility() const
{
	return bShowCones;
}

int FVASoundSource::GetSoundSourceID() const
{
	return SoundSourceID;
}

int FVASoundSource::GetPlayState() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return -1;
	}

	return ActiveBuffer->GetSoundBufferAction();
}

float FVASoundSource::GetPower() const
{
	return Power;
}

FVADirectivity* FVASoundSource::GetDirectivity() const
{
	return Directivity;
}


UVASourceComponent* FVASoundSource::GetParentComponent() const
{
	return ParentComponent;
}









