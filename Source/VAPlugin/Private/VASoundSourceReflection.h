// Sound Source Reflection Class 

#pragma once

#include <string>

#include "CoreMinimal.h"

class AVASoundSourceRepresentation;
class FVASoundSource;
class AVAReflectionWall;
class FVADirectivity;

class FVASoundSourceReflection 
{
public:

	// Calculates the correct position by its own, needs the position of the original soundSource
	FVASoundSourceReflection(FVASoundSource* ParentSourceN, AVAReflectionWall* WallN,
	                        const std::string& NameN = "", const FVector& PosN = FVector(),
	                        const FRotator& RotN = FRotator(), float PowerN = 0.0f);

	bool SetPosition(FVector PosN);
	bool SetRotation(FRotator RotN);
	bool SetDirectivity(FVADirectivity* Dir);
	bool RemoveDirectivity();
	
	void SetVisibility(bool bVis);
	bool SetPowerAccordingOrigSource();

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
