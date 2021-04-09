#include "SoundSource/VASoundSourceBase.h"

#include "VAPlugin.h"
#include "VAUtils.h"
#include "VADefines.h"
#include "Directivity/VADirectivity.h"

#include "SoundSource/VASoundSourceRepresentation.h"

#include "Engine/World.h"


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSourceBase::FVASoundSourceBase(UWorld* World, const FVector& Position, const FRotator& Rotation, float Power, const std::string& Name /* = "" */, FVADirectivity* Directivity /* = nullptr */)
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
	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVASoundSourceBase::FVASoundSourceBase()]: Error initializing VA sound source", true);
		return;
	}
	if (Directivity != nullptr)
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

FVASoundSourceBase::~FVASoundSourceBase()
{
	if(SoundSourceRepresentation && SoundSourceRepresentation->IsValidLowLevel())
	{
		SoundSourceRepresentation->Destroy();
	}
}



// ****************************************************************** // 
// ******* Setter *************************************************** //
// ****************************************************************** //


void FVASoundSourceBase::SetPosition(const FVector NewPosition)
{
	Position = NewPosition;

	if (!FVAPlugin::SetSoundSourcePosition(SoundSourceID, Position))
	{
		FVAUtils::LogStuff("[FVASoundSourceBase::SetPosition()]:" +
			FString(" Could not set sound source position in VA. Position of visual and auditive representation might mismatch now."), true);
	}

	SoundSourceRepresentation->SetPosition(Position);
}

void FVASoundSourceBase::SetRotation(const FRotator NewRotation)
{
	Rotation = NewRotation;

	if (!FVAPlugin::SetSoundSourceRotation(SoundSourceID, Rotation))
	{
		FVAUtils::LogStuff("[FVASoundSourceBase::SetRotation()]:" +
			FString(" Could not set sound source rotation in VA. Orientation of visual and auditive representation might mismatch now."), true);
	}
	
	SoundSourceRepresentation->SetRotation(Rotation);
}

void FVASoundSourceBase::SetVisibility(const bool bVisible)
{
	if (bShowCones == bVisible)
	{
		return;
	}
	
	bShowCones = bVisible;
	SoundSourceRepresentation->SetVisibility(bShowCones);
}

bool FVASoundSourceBase::SetDirectivity(FVADirectivity* NewDirectivity)
{
	if (NewDirectivity == nullptr)
	{
		FVAUtils::OpenMessageBox(FString("[FVASoundSourceBase::SetDirectivity]: Cannot set empty directivity, use RemoveDirectivity() instead"), true);
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

bool FVASoundSourceBase::RemoveDirectivity()
{
	if(FVAPlugin::RemoveSoundSourceDirectivity(SoundSourceID))
	{
		Directivity = nullptr;
		return true;
	}
	return false;
}

bool FVASoundSourceBase::SetSignalSource(const std::string& NewSignalSourceID)
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

bool FVASoundSourceBase::SetPower(const float PowerN)
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

bool FVASoundSourceBase::MuteSound(const bool MutedN)
{
	return FVAPlugin::SetSoundSourceMuted(SoundSourceID, MutedN);
}

// ****************************************************************** // 
// ******* Getter *************************************************** //
// ****************************************************************** //

int FVASoundSourceBase::GetSoundSourceID() const
{
	return SoundSourceID;
}

const std::string& FVASoundSourceBase::GetSoundSourceName() const
{
	return Name;
}

FVector FVASoundSourceBase::GetPosition() const
{
	return Position;
}

FRotator FVASoundSourceBase::GetRotation() const
{
	return Rotation;
}

bool FVASoundSourceBase::GetVisibility() const
{
	return bShowCones;
}


float FVASoundSourceBase::GetPower() const
{
	return Power;
}

FVADirectivity* FVASoundSourceBase::GetDirectivity() const
{
	return Directivity;
}

FString FVASoundSourceBase::GetDirectivityFilename() const
{
	if (!Directivity)
	{
		return FString("");
	}

	return Directivity->GetFileName();
}

const std::string& FVASoundSourceBase::GetSignalSourceID() const
{
	return SignalSourceID;
}
