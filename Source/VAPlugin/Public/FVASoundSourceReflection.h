// Sound Source Reflection Class 

#pragma once

#include <string>

#include "VASoundSourceRepresentation.h"
#include "VAReflectionWall.h"
#include "FVASoundSource.h"
#include "VADirectivity.h"


class FVASoundSource;

class FVASoundSourceReflection // : public VASoundSourceBaseClass
{
public:

	// Calculates the correct position by its own, needs the position of the original soundSource
	FVASoundSourceReflection(FVASoundSource* ParentSourceN, AVAReflectionWall* WallN,
	                        std::string BufferIDN, std::string NameN = "", FVector PosN = FVector(),
	                        FRotator RotN = FRotator(), float PowerN = 0.0f);

	void SetPosition(FVector PosN);
	void SetRotation(FRotator RotN);
	void SetVisibility(bool bVis);
	void SetDirectivity(FVADirectivity* Dir) const;
	void SetPowerAccordingOrigSource();

	FVector GetPosition() const;
	FRotator GetRotation() const;
	int GetSoundSourceID() const;




private:

	// parent Sound Source (where the reflection is created from)
	FVASoundSource* ParentSource;

	// Wall this Reflection is mirrored on
	AVAReflectionWall* Wall;

	int SoundSourceID;
	std::string SoundSourceName;

	float Power;

	FVector Pos;
	FRotator Rot;


	// Sound Source Representation
	AVASoundSourceRepresentation* SoundSourceRepresentation;


	bool bShowCones;
};
