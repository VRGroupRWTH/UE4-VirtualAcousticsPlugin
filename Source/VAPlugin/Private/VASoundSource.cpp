#include "VASoundSource.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "VASourceComponent.h"
#include "VASoundSourceReflection.h"
#include "VASoundSourceRepresentation.h"
#include "VADirectivity.h"
#include "VAReflectionWall.h"

#include "SignalSources/VAJetEngineSignalSource.h"
#include "SignalSources/VAAudiofileSignalSource.h"

int FVASoundSource::Counter = 0;

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSource::FVASoundSource(UVASourceComponent* ParentComponent, TArray<AVAReflectionWall*> WallArray) :
	ParentComponent(ParentComponent)
{
	bShowCones = FVAPlugin::GetDebugMode();
	bHandleReflections = ParentComponent->GetHandleReflections();
	Power = ParentComponent->GetSoundPower();

	Position = ParentComponent->GetPosition();
	Rotation = ParentComponent->GetRotation();

	const std::string Name = "SoundSource";

	if (FVAPlugin::GetIsMaster())
	{
		SoundSourceID = FVAPlugin::CreateNewSoundSource(Name, Position, Rotation, Power);
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
			
			const float R = EntryWall->GetReflectionValueR();
			const float PowerR = Power * R * R;
			ImageSources.Add(new FVASoundSourceReflection(this, EntryWall, NameTmp, Position, Rotation, PowerR));
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
		for (auto EntryReflections : ImageSources)
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
		for (auto EntryReflections : ImageSources)
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

	if (bHandleReflections && ImageSources.Num() != 0)
	{
		for (auto EntryReflections : ImageSources)
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
		for (auto EntryReflections : ImageSources)
		{
			if(!EntryReflections->SetDirectivity(DirN))
			{
				bFullSuccess = false;
			}
		}
	}

	return bFullSuccess;
}

bool FVASoundSource::SetSignalSourceToImageSources(const std::string& SignalSourceID)
{
	bool bFullSuccess = true;
	if (bHandleReflections)
	{
		for (auto EntryReflections : ImageSources)
		{
			if (!FVAPlugin::SetSoundSourceSignalSource(EntryReflections->GetSoundSourceID(), SignalSourceID))
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
		for (auto EntryReflections : ImageSources)
		{
			if (!EntryReflections->RemoveDirectivity())
			{
				bFullSuccess = false;
			}
		}
	}

	return bFullSuccess;
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
	
	for (auto EntryReflections : ImageSources)
	{
		if(!EntryReflections->SetPowerAccordingOrigSource())
		{
			bFullSuccess = false;
		}
	}
	return bFullSuccess;
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

	for (auto EntryReflections : ImageSources)
	{
		if(!FVAPlugin::SetSoundSourceMuted(EntryReflections->GetSoundSourceID(), MutedN))
		{
			bFullSuccess = false;
		}
	}
	return bFullSuccess;
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









