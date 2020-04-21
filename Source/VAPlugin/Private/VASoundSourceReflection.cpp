#include "VASoundSourceReflection.h"
#include "VAPlugin.h"



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
			FVAUtils::LogStuff(
				"[VASoundSourceReflection::FVASoundSourceReflection(...)] Error initializing soundSource in VASoundSourceReflection()",
				true);
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

void FVASoundSourceReflection::SetPosition(const FVector PosN)
{
	Pos = FVAUtils::ComputeReflectedPos(Wall, PosN);

	if (FVAPlugin::GetIsMaster())
	{
		FVAPlugin::SetSoundSourcePosition(SoundSourceID, Pos);
	}

	if (bShowCones)
	{
		SoundSourceRepresentation->SetPosition(Pos);
	}
}

void FVASoundSourceReflection::SetRotation(const FRotator RotN)
{
	Rot = FVAUtils::ComputeReflectedRot(Wall, RotN);

	if (FVAPlugin::GetIsMaster())
	{
		FVAPlugin::SetSoundSourceRotation(SoundSourceID, Rot);
	}

	SoundSourceRepresentation->SetRotation(Rot);

}

void FVASoundSourceReflection::SetVisibility(const bool bVis)
{
	bShowCones = bVis;
	SoundSourceRepresentation->SetVisibility(bVis);
}

void FVASoundSourceReflection::SetDirectivity(FVADirectivity* Dir) const
{
	FVAPlugin::SetSoundSourceDirectivity(SoundSourceID, Dir->GetID());
}

void FVASoundSourceReflection::SetPowerAccordingOrigSource()
{
	if (FVAPlugin::GetIsMaster())
	{
		const float R = Wall->GetReflectionValueR();
		Power = ParentSource->GetPower() * R * R;
		FVAPlugin::SetSoundSourcePower(SoundSourceID, Power);
	}
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
