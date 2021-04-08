// Class for a Sound Source which can handle its reflections 

#pragma once

#include "CoreMinimal.h"

#include <string>

class AVASoundSourceRepresentation;
class UWorld;

class FVASoundSourceBase
{
public:

	// Initialization
	FVASoundSourceBase(UWorld* World, const FVector& PosN, const FRotator& RotN, float PowerN, int DirectivityID = -1, const std::string& NameN = "");
	~FVASoundSourceBase();

	// Setter
	void SetPosition(FVector NewPosition);
	void SetRotation(FRotator NewRotation);
	void SetVisibility(bool bVisible);
	bool SetDirectivity(int DirectivityID);

	bool RemoveDirectivity();

	bool SetPower(float PowerN);

	bool MuteSound(bool MutedN = true);

	

	// Getter
	int GetSoundSourceID() const;
	const std::string& GetSoundSourceName() const;

	FVector GetPosition() const;
	FRotator GetRotation() const;

	bool GetVisibility() const;

	float GetPower() const;
	int GetDirectivityID() const;

private:

	int SoundSourceID;
	std::string Name;

	FVector Position;
	FRotator Rotation;

	bool bHandleReflections;
	bool bShowCones;

	float Power;
	int DirectivityID;
	
	AVASoundSourceRepresentation* SoundSourceRepresentation;

};
