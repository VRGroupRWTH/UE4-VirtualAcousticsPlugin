#include "VASoundSourceReflection.h"

#include "VAPlugin.h"
#include "VAUtils.h"

#include "VASoundSource.h"
#include "VAReflectionWall.h"
#include "VASoundSourceRepresentation.h"
#include "VADirectivity.h"

// ****************************************************************** // 
// ******* Initialization ******************************************* //
// ****************************************************************** //

FVASoundSourceReflection::FVASoundSourceReflection(FVASoundSource* ParentSourceN,
	AVAReflectionWall* WallN, const std::string BufferIDN, const std::string NameN, 
	const FVector PosN, const FRotator RotN, const float PowerN) :
	ParentSource(ParentSourceN),
	Wall(WallN),
	Power(PowerN)
{
	bShowCones = ParentSource->GetVisibility();

	Pos = FVAUtils::ComputeReflectedPos(Wall, PosN);
	Rot = FVAUtils::ComputeReflectedRot(Wall, RotN);

	if (FVAPlugin::GetIsMaster())
	{
		SoundSourceID = FVAPlugin::CreateNewSoundSource(BufferIDN, NameN, Pos, Rot, Power);
		if (SoundSourceID == -1)
		{
			FVAUtils::LogStuff("[FVASoundSourceReflection::FVASoundSourceReflection()]:" + 
				FString(" Error initializing soundSource in VASoundSourceReflection()"),	true);
			return;
		}
		
		SetPowerAccordingOrigSource();
	}

	SoundSourceRepresentation = ParentSource->GetParentComponent()->GetWorld()->SpawnActor<AVASoundSourceRepresentation
	>(AVASoundSourceRepresentation::StaticClass());


	SoundSourceRepresentation->SetPosition(Pos);
	SoundSourceRepresentation->SetRotation(Rot);
	SoundSourceRepresentation->SetVisibility(bShowCones);
}


// ****************************************************************** // 
// ******* Setter *************************************************** //
// ****************************************************************** //

bool FVASoundSourceReflection::SetPosition(const FVector PosN)
{
	Pos = FVAUtils::ComputeReflectedPos(Wall, PosN);

	bool Ret = false;
	
	if (FVAPlugin::GetIsMaster())
	{
		Ret = FVAPlugin::SetSoundSourcePosition(SoundSourceID, Pos);
	}

	if (bShowCones)
	{
		SoundSourceRepresentation->SetPosition(Pos);
	}

	return Ret;
}

bool FVASoundSourceReflection::SetRotation(const FRotator RotN)
{
	Rot = FVAUtils::ComputeReflectedRot(Wall, RotN);

	bool Ret = false;
	
	if (FVAPlugin::GetIsMaster())
	{
		Ret = FVAPlugin::SetSoundSourceRotation(SoundSourceID, Rot);
	}

	SoundSourceRepresentation->SetRotation(Rot);

	return Ret;
}

bool FVASoundSourceReflection::SetDirectivity(FVADirectivity* Dir) const
{
	return FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, Dir->GetID());
}

void FVASoundSourceReflection::SetVisibility(const bool bVis)
{
	bShowCones = bVis;
	SoundSourceRepresentation->SetVisibility(bVis);
}

bool FVASoundSourceReflection::SetPowerAccordingOrigSource()
{
	if (!FVAPlugin::GetIsMaster())
	{
		return false;
	}
	
	const float R = Wall->GetReflectionValueR();
	Power = ParentSource->GetPower() * R * R;
	return FVAPlugin::SetSoundSourcePower(SoundSourceID, Power);
}


// ****************************************************************** // 
// ******* Getter *************************************************** //
// ****************************************************************** //

FVector FVASoundSourceReflection::GetPosition() const
{
	return Pos;
}

FRotator FVASoundSourceReflection::GetRotation() const
{
	return Rot;
}

int FVASoundSourceReflection::GetSoundSourceID() const
{
	return SoundSourceID;
}
