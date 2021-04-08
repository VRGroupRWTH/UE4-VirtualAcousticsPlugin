#include "VASoundSourceBase.h"

#include "VAPlugin.h"
#include "VAUtils.h"
#include "VADefines.h"

#include "VASoundSourceRepresentation.h"

#include "Engine/World.h"


// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSourceBase::FVASoundSourceBase(UWorld* World, const FVector& Position, const FRotator& Rotation, float Power, int DirectivityID /* = -1 */, const std::string& Name /* = "" */)
	: SoundSourceID(VA_INVALID_ID)
	, Name(Name)
	, Position(Position)
	, Rotation(Rotation)
	, bShowCones(false)
	, Power(Power)
	, DirectivityID(VA_INVALID_ID)
{
	bShowCones = FVAPlugin::GetDebugMode();

	SoundSourceID = FVAPlugin::CreateNewSoundSource(Name, Position, Rotation, Power);
	if (SoundSourceID == -1)
	{
		FVAUtils::LogStuff("[FVASoundSourceBase::FVASoundSourceBase()]: Error initializing VA sound source", true);
		return;
	}
	if (VA::IsValidID(DirectivityID))
	{
		SetDirectivity(DirectivityID);
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

bool FVASoundSourceBase::SetDirectivity(int NewDirectivityID)
{
	if (DirectivityID == NewDirectivityID)
	{
		return true;
	}

	if (FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, DirectivityID))
	{
		DirectivityID = NewDirectivityID;
		return true;
	}
	return false;
}

bool FVASoundSourceBase::RemoveDirectivity()
{
	return FVAPlugin::RemoveSoundSourceDirectivity(SoundSourceID);
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

int FVASoundSourceBase::GetDirectivityID() const
{
	return DirectivityID;
}
