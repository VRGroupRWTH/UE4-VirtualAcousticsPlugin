#include "SoundSource/VASoundSource.h"

#include "VAPlugin.h"
#include "VAUtils.h"
#include "VADefines.h"
#include "Directivity/VADirectivity.h"

#include "SoundSource/VASoundSourceRepresentation.h"

#include "Engine/World.h"


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSource::FVASoundSource(UWorld* World, const FVector& Position, const FRotator& Rotation, float Power, const std::string& Name /* = "" */, TSharedPtr<FVADirectivity> Directivity /* = nullptr */)
	: SoundSourceID(VA_INVALID_ID)
	, Name(Name)
	, Position(Position)
	, Rotation(Rotation)
	, bShowCones(false)
	, Power(Power)
	, Directivity(nullptr)
	, SignalSourceID(VA_INVALID_ID_STRING)
{
	bShowCones = FVAPlugin::GetDebugMode();

	SoundSourceID = FVAPlugin::CreateNewSoundSource(Name, Position, Rotation, Power);
	if (SoundSourceID == VA_INVALID_ID)
	{
		FVAUtils::LogStuff("[FVASoundSource::FVASoundSource()]: Error initializing VA sound source", true);
		return;
	}
	if (Directivity.IsValid())
	{
		SetDirectivity(Directivity);
	}

	// Show graphical representation
	if (World)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.bAllowDuringConstructionScript = true;
		SoundSourceRepresentation = World->SpawnActor<AVASoundSourceRepresentation>(
			AVASoundSourceRepresentation::StaticClass(), SpawnParameters);


		SoundSourceRepresentation->SetPosition(Position);
		SoundSourceRepresentation->SetRotation(Rotation);
		SoundSourceRepresentation->SetVisibility(bShowCones);
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


void FVASoundSource::SetPosition(const FVector NewPosition)
{
	Position = NewPosition;

	if (!FVAPlugin::SetSoundSourcePosition(SoundSourceID, Position))
	{
		FVAUtils::LogStuff("[FVASoundSource::SetPosition()]:" +
			FString(" Could not set sound source position in VA. Position of visual and auditive representation might mismatch now."), true);
	}

	SoundSourceRepresentation->SetPosition(Position);
}

void FVASoundSource::SetRotation(const FRotator NewRotation)
{
	Rotation = NewRotation;

	if (!FVAPlugin::SetSoundSourceRotation(SoundSourceID, Rotation))
	{
		FVAUtils::LogStuff("[FVASoundSource::SetRotation()]:" +
			FString(" Could not set sound source rotation in VA. Orientation of visual and auditive representation might mismatch now."), true);
	}
	
	SoundSourceRepresentation->SetRotation(Rotation);
}

void FVASoundSource::SetVisibility(const bool bVisible)
{
	if (bShowCones == bVisible)
	{
		return;
	}
	
	bShowCones = bVisible;
	SoundSourceRepresentation->SetVisibility(bShowCones);
}

bool FVASoundSource::SetDirectivity(TSharedPtr<FVADirectivity> NewDirectivity)
{
	if (!NewDirectivity.IsValid())
	{
		FVAUtils::OpenMessageBox(FString("[FVASoundSource::SetDirectivity]: Cannot set empty directivity, use RemoveDirectivity() instead"), true);
		return false;
	}
	if (Directivity == NewDirectivity)
	{
		return true;
	}

	if (FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, NewDirectivity->GetID()))
	{
		Directivity = NewDirectivity;
		return true;
	}
	return false;
}

bool FVASoundSource::RemoveDirectivity()
{
	if(FVAPlugin::RemoveSoundSourceDirectivity(SoundSourceID))
	{
		Directivity.Reset();
		return true;
	}
	return false;
}

bool FVASoundSource::SetSignalSource(const std::string& NewSignalSourceID)
{
	if (SignalSourceID == NewSignalSourceID)
	{
		return true;
	}
	if (FVAPlugin::SetSoundSourceSignalSource(SoundSourceID, NewSignalSourceID))
	{
		SignalSourceID = NewSignalSourceID;
		return true;
	}
	return false;
}

bool FVASoundSource::SetPower(const float PowerN)
{
	if (Power == PowerN)
	{
		return true;
	}
	
	if (FVAPlugin::SetSoundSourcePower(SoundSourceID, PowerN))
	{
		Power = PowerN;
		return true;
	}
	return false;
}

bool FVASoundSource::MuteSound(const bool MutedN)
{
	return FVAPlugin::SetSoundSourceMuted(SoundSourceID, MutedN);
}

// ****************************************************************** // 
// ******* Getter *************************************************** //
// ****************************************************************** //

int FVASoundSource::GetSoundSourceID() const
{
	return SoundSourceID;
}

const std::string& FVASoundSource::GetSoundSourceName() const
{
	return Name;
}

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


float FVASoundSource::GetPower() const
{
	return Power;
}

TSharedPtr<FVADirectivity> FVASoundSource::GetDirectivity() const
{
	return Directivity;
}

FString FVASoundSource::GetDirectivityFilename() const
{
	if (!Directivity)
	{
		return FString("");
	}

	return Directivity->GetFileName();
}

const std::string& FVASoundSource::GetSignalSourceID() const
{
	return SignalSourceID;
}
