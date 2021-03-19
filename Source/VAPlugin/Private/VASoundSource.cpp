#include "VASoundSource.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "VASourceComponent.h"
#include "VASoundSourceReflection.h"
#include "VASoundSourceRepresentation.h"
#include "VADirectivity.h"
#include "VAReflectionWall.h"

int FVASoundSource::Counter = 0;

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSource::FVASoundSource(UVASourceComponent* ParentComponent, TArray<AVAReflectionWall*> WallArray) :
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
		std::string sSignalID = "-1";
		if ( ParentComponent->GetSignalSourceType() == ESignalSource::JetEngine )
		{
			sSignalID = FVAPlugin::CreateSignalSourcePrototype( ESignalSource::JetEngine );
			//FVAPlugin::SetJetEngineRMP(sSignalID, ParentComponent->GetJetRPM());
		}
		else
		{
			ActiveBuffer = BufferManager.GetBufferByFileName(ParentComponent->GetSoundFile());

			if (ActiveBuffer == nullptr)
			{
				FVAUtils::LogStuff("[FVASoundSource::VASoundSource()]: Error initializing Buffer", true);
				return;
			}

			ActiveBuffer->SetLoop(bLoop);

			// Only Change if necessary
			if (SoundTimeOffset > 0.0f)
			{
				ActiveBuffer->SetSoundTimeOffset(SoundTimeOffset);
			}
			sSignalID = ActiveBuffer->GetID();
		}

		SoundSourceID = FVAPlugin::CreateNewSoundSource(sSignalID, Name, Position, Rotation, Power);
		if (SoundSourceID == -1)
		{
			FVAUtils::LogStuff("[FVASoundSource::VASoundSource()]: Error initializing soundSource",
			                  true);
			return;
		}

		FVAPlugin::SetSoundSourcePower(SoundSourceID, ParentComponent->GetSoundPower());
	}

	// Show graphical representation
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bAllowDuringConstructionScript = true;
	SoundSourceRepresentation = ParentComponent->GetWorld()->SpawnActor<AVASoundSourceRepresentation>(
		AVASoundSourceRepresentation::StaticClass(), SpawnParameters);


	SoundSourceRepresentation->SetPosition(ParentComponent->GetPosition());
	SoundSourceRepresentation->SetRotation(ParentComponent->GetRotation());
	SoundSourceRepresentation->SetVisibility(bShowCones);

	if (bHandleReflections)
	{
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

FVASoundSource::~FVASoundSource()
{
	if(SoundSourceRepresentation && SoundSourceRepresentation->IsValidLowLevel())
	{
		SoundSourceRepresentation->Destroy();
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


	bool bFullSuccess = true;
	
	SoundSourceRepresentation->SetVisibility(bShowCones);

	if (bHandleReflections && Reflections.Num() != 0)
	{
		for (auto EntryReflections : Reflections)
		{
			EntryReflections->SetVisibility(bShowCones);
		}
	}
}

bool FVASoundSource::SetDirectivity(FVADirectivity* DirN)
{
	Directivity = DirN;

	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}


	bool bFullSuccess = true;
	
	if(!FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, DirN->GetID()))
	{
		bFullSuccess = false;
	}

	if (bHandleReflections)
	{
		for (auto EntryReflections : Reflections)
		{
			if(!EntryReflections->SetDirectivity(DirN))
			{
				bFullSuccess = false;
			}
		}
	}

	return bFullSuccess;
}

bool FVASoundSource::RemoveDirectivity()
{
	Directivity = nullptr;
	
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}


	bool bFullSuccess = true;

	if (!FVAPlugin::RemoveSoundSourceDirectivity(SoundSourceID))
	{
		bFullSuccess = false;
	}

	if (bHandleReflections)
	{
		for (auto EntryReflections : Reflections)
		{
			if (!EntryReflections->RemoveDirectivity())
			{
				bFullSuccess = false;
			}
		}
	}

	return bFullSuccess;
}

bool FVASoundSource::SetPlayAction(const int ActionN) const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}
	
	if (int(GetPlayState()) == ActionN)
	{
		return true;
	}

	return ActiveBuffer->SetSoundBufferAction(ActionN);
}

bool FVASoundSource::SetSoundTime(const float TimeN) const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	return ActiveBuffer->SetSoundTimeOffset(TimeN);
}

bool FVASoundSource::SetLoop(const bool bLoopN)
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

	return ActiveBuffer->SetLoop(bLoopN);
}

bool FVASoundSource::SetPower(const float PowerN)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (Power == PowerN)
	{
		return true;
	}
	
	Power = PowerN;

	bool bFullSuccess = true;
	
	if(!FVAPlugin::SetSoundSourcePower(SoundSourceID, PowerN))
	{
		bFullSuccess = false;
	}
	
	for (auto EntryReflections : Reflections)
	{
		if(!EntryReflections->SetPowerAccordingOrigSource())
		{
			bFullSuccess = false;
		}
	}
	return bFullSuccess;
}

bool FVASoundSource::PlaySound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	return SetPlayAction(EPlayAction::Type::Play);
}

bool FVASoundSource::StopSound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	return SetPlayAction(EPlayAction::Type::Stop);
}

bool FVASoundSource::PauseSound() const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	return SetPlayAction(EPlayAction::Type::Pause);
}

bool FVASoundSource::PlaySoundFromSecond(const float TimeN) const
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	if (SetSoundTime(TimeN))
	{
		return SetPlayAction(EPlayAction::Type::Play);
	}
	
	return false;
}

bool FVASoundSource::MuteSound(const bool MutedN)
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}

	bool bFullSuccess = true;
	if(!FVAPlugin::SetSoundSourceMuted(SoundSourceID, MutedN))
	{
		bFullSuccess = false;
	}

	for (auto EntryReflections : Reflections)
	{
		if(!FVAPlugin::SetSoundSourceMuted(EntryReflections->GetSoundSourceID(), MutedN))
		{
			bFullSuccess = false;
		}
	}
	return bFullSuccess;
}

bool FVASoundSource::SetNewSound(FString SoundFileN)
{
	// create buffer
	FVASignalBuffer* TmpBuffer = BufferManager.GetBufferByFileName(SoundFileN);

	// Check if is valid
	if (TmpBuffer == nullptr)
	{
		FVAUtils::LogStuff("[FVASoundSource::SetNewSound()]: Buffer from file " + 
			SoundFileN + " was loaded incorrectly!", true);
		return false;
	}

	// Stop 

	// If the same Buffer is already loaded only stop sound (prev)
	if (TmpBuffer == ActiveBuffer)
	{
		StopSound();
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









